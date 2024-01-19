#include "homeokinesis_controller.hpp"

using namespace are;
using namespace are::hk;

void Homeokinesis::init(int nb_inputs, int nb_outputs){
    _nbr_inputs = nb_inputs;
    _nbr_outputs = nb_outputs;

    A.Identity(nb_inputs,nb_outputs);
    S.Identity(nb_inputs,nb_inputs);
    C.Identity(nb_outputs,nb_inputs);
    b.Zero(nb_inputs,1);
    h.Zero(nb_outputs,1);
    L.Zero(nb_inputs,nb_inputs);
    v_avg.Zero(nb_inputs,1);
    A_native.Identity(nb_inputs,nb_outputs);
    C_native.Identity(nb_outputs,nb_inputs);

    R.Zero(nb_inputs,nb_inputs);

    C*=_conf.initFeedbackStrength;
    S*=0.05f;
    C_native*=1.2f;

    y_teaching.Zero(nb_outputs,1);
    x.Zero(nb_inputs,1);
    x_smooth.Zero(nb_inputs,1);
    x_buffer.resize(_conf.buffersize);
    y_buffer.resize(_conf.buffersize);
    for (size_t k = 0; k < _conf.buffersize; k++) {
        x_buffer[k].Zero(nb_inputs,1);
        y_buffer[k].Zero(nb_outputs,1);
    }

}

std::vector<double> Homeokinesis::update(const std::vector<double> &sensorValues){
    //step 1: learn
    learn();
    //step 2: step
    misc::stdvect_to_eigenmat(sensorValues,x);
    Matrix y;
    step(x,y);
    std::vector<double> ctrl_cmd;
    //misc::eigenmat_to_stdvect(y,ctrl_cmd);
    return ctrl_cmd;
}

void Homeokinesis::step(const Matrix &x,  Matrix &y){

    // averaging over the last s4avg values of x_buffer
    _conf.steps4Averaging = std::clamp<size_t>(_conf.steps4Averaging,1,_conf.buffersize-1);
    if(_conf.steps4Averaging > 1) //TODO: restart from there.
      x_smooth += (x - x_smooth)*(1.0/_conf.steps4Averaging);
    else
      x_smooth = x;

    x_buffer[t%_conf.buffersize] = x_smooth; // we store the smoothed sensor value

    // calculate controller values based on current input values (smoothed)
    y = _matrix_map<double(double)>(C*(x_smooth + (v_avg*creativity)) + h,tanh);

    // Put new output vector in ring buffer y_buffer
    y_buffer[t%_conf.buffersize] = y;

    // convert y to motor*
    //y.convertToBuffer(y, number_motors);

    // update step counter
    t++;
}

void Homeokinesis::learn(){
    using std::tanh;
    // the effective x/y is (actual-steps4delay) element of buffer
    int s4delay = std::clamp<int>(_conf.steps4Delay,1,_conf.buffersize-1);
    const Matrix& x       = x_buffer[(t - std::max(s4delay,1) + _conf.buffersize) % _conf.buffersize];
    const Matrix& y_creat = y_buffer[(t - std::max(s4delay,1) + _conf.buffersize) % _conf.buffersize];
    const Matrix& x_fut   = x_buffer[t% _conf.buffersize]; // future sensor (with respect to x,y)

    const Matrix& xi      = x_fut  - (A * y_creat + b + S * x); // here we use creativity

    const Matrix& z       = (C * (x) + h); // here no creativity
    const Matrix& y       = z.array().tanh();
    Matrix g_prime = z;
    _tanh_diff(g_prime);

    L = A * C.cwiseProduct(g_prime) + S;
    R = A * C+S; // this is only used for visualization

    const Matrix& eta    = A.completeOrthogonalDecomposition().pseudoInverse() * xi;
    const Matrix& y_hat  = y + eta*causeaware;

    const Matrix& Lplus  = L.completeOrthogonalDecomposition().pseudoInverse();
    const Matrix& v      = Lplus * xi;
    const Matrix& chi    = Lplus.transpose() * v;

    const Matrix& mu     = A.transpose().cwiseProduct(g_prime) * chi;
    const Matrix& epsrel = (mu.array().colwise() * (C * v).array()) * (sense * 2);

    const Matrix& v_hat = v + x * harmony;
    auto clip_01 = [](double x)->double{return clip(0.1,x);};
    auto clip_005 = [](double x)->double{return clip(0.05,x);};
    auto power3 = [](double x) -> double{return x*x*x;};

    v_avg += ( v  - v_avg ) *.1;

    double EE = 1.0;
    if(loga){
      EE = .1/(v.squaredNorm() + .001); // logarithmic error (E = log(v^T v))
    }
    if(epsA > 0){
        double epsS=epsA*_conf.factorS;
        double epsb=epsA*_conf.factorb;
        A += _matrix_map<double(double)>(xi * (y_hat.transpose()) * epsA,clip_01);
        if(damping)
            A += _matrix_map<double(double)>(_matrix_map<double(double)>(A_native-A,power3)*damping,clip_01);

        if(_conf.useExtendedModel)
            S += _matrix_map<double(double)>(xi * (x.transpose())     * (epsS)+ (S *  -damping*10),clip_01);
        b += _matrix_map<double(double)>(xi * (epsb) + (b *  -damping), clip_01);
    }
    if(epsC > 0){
      C += _matrix_map<double(double)>(((mu * v_hat.transpose()).array() - (epsrel.array().colwise() * y.array()) * x.transpose().array())   * (EE * epsC), clip_01);
      if(damping)
        C += _matrix_map<double(double)>(_matrix_map<double(double)>(C_native-C,power3)*damping, clip_005);
      h += _matrix_map<double(double)>(((mu*harmony).array() - (epsrel.array().colwise() * y.array())) * (EE * epsC * _conf.factorh),clip_005);

      if(intern_isTeaching && gamma > 0){
        // scale of the additional terms
        const Matrix& metric = (A.transpose()) * (Lplus.transpose()*Lplus) * A;

        const Matrix& y      = y_buffer[(t-1)% _conf.buffersize];
        const Matrix& xsi    = y_teaching - y;
        const Matrix& delta  = xsi.array().colwise() * g_prime.array();
        C += _matrix_map<double(double)>((metric * delta*(x.transpose()) ) * (gamma * epsC),clip_005);
        h += _matrix_map<double(double)>((metric * delta)        * (gamma * epsC * _conf.factorh),clip_005);
        // after we applied teaching signal it is switched off until new signal is given
        intern_isTeaching    = false;
      }
    }
}

void Homeokinesis::_set_default_config(){
    _conf.initFeedbackStrength = 1.0;
    _conf.useExtendedModel     = true;
    _conf.useTeaching          = false;
    _conf.steps4Averaging      = 1;
    _conf.steps4Delay          = 1;
    _conf.someInternalParams   = false;
    _conf.onlyMainParameters   = true;
    _conf.buffersize = 10;

    _conf.factorS              = 1;
    _conf.factorb              = 1;
    _conf.factorh              = 1;
}
