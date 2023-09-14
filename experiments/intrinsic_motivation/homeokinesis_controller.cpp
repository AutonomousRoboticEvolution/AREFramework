#include "homeokinesis_controller.hpp"

using namespace are;

void Homeokinesis::init(int nb_inputs, int nb_outputs){

}

std::vector<double> Homeokinesis::update(const std::vector<double> &sensorValues){
        //step 1: learn
        //step 2: step
}

void step(const std::vector<double> &sensors,  std::vector<double> &motors){
    misc::stdvect_to_eigenvect(sensors,x);

    // averaging over the last s4avg values of x_buffer
    conf.steps4Averaging = ::clip(conf.steps4Averaging,1,buffersize-1);
    if(conf.steps4Averaging > 1) //TODO: restart from there.
      x_smooth += (x - x_smooth)*(1.0/conf.steps4Averaging);
    else
      x_smooth = x;

    x_buffer[t%buffersize] = x_smooth; // we store the smoothed sensor value

    // calculate controller values based on current input values (smoothed)
    Matrix y =   (C*(x_smooth + (v_avg*creativity)) + h).map(g);

    // Put new output vector in ring buffer y_buffer
    y_buffer[t%buffersize] = y;

    // convert y to motor*
    y.convertToBuffer(y_, number_motors);

    // update step counter
    t++;
}

void learn(){
    using Matrix = Eigen::MatrixXd;

    // the effective x/y is (actual-steps4delay) element of buffer
    int s4delay = ::clip(conf.steps4Delay,1,buffersize-1);
    const Vector& x       = x_buffer[(t - max(s4delay,1) + buffersize) % buffersize];
    const Matrix& y_creat = y_buffer[(t - max(s4delay,1) + buffersize) % buffersize];
    const Matrix& x_fut   = x_buffer[t% buffersize]; // future sensor (with respect to x,y)

    const Matrix& xi      = x_fut  - (A * y_creat + b + S * x); // here we use creativity

    const Matrix& z       = (C * (x) + h); // here no creativity
    const Matrix& y       = z.map(g);
    const Matrix& g_prime = z.map(g_s);

    L = A * (C & g_prime) + S;
    R = A * C+S; // this is only used for visualization

    const Matrix& eta    = A.pseudoInverse() * xi; //TODO: pseudo inverse
    const Matrix& y_hat  = y + eta*causeaware;

    const Matrix& Lplus  = pseudoInvL(L,A,C); //TODO: pseudo inverse left
    const Matrix& v      = Lplus * xi;
    const Matrix& chi    = (Lplus^T) * v;

    const Matrix& mu     = ((A^T) & g_prime) * chi;
    const Matrix& epsrel = (mu & (C * v)) * (sense * 2);

    const Matrix& v_hat = v + x * harmony;

    v_avg += ( v  - v_avg ) *.1;

    double EE = 1.0;
    if(loga){
      EE = .1/(v.norm_sqr() + .001); // logarithmic error (E = log(v^T v))
    }
    if(epsA > 0){
      double epsS=epsA*conf.factorS;
      double epsb=epsA*conf.factorb;
      A   += (xi * (y_hat^T) * epsA                      ).mapP(0.1, clip);
      if(damping)
        A += (((A_native-A).map(power3))*damping         ).mapP(0.1, clip);
      if(conf.useExtendedModel)
        S += (xi * (x^T)     * (epsS)+ (S *  -damping*10) ).mapP(0.1, clip);
      b   += (xi             * (epsb) + (b *  -damping)    ).mapP(0.1, clip);
    }
    if(epsC > 0){
      C += (( mu * (v_hat^T)
              - (epsrel & y) * (x^T))   * (EE * epsC) ).mapP(.05, clip);
      if(damping)
        C += (((C_native-C).map(power3))*damping      ).mapP(.05, clip);
      h += ((mu*harmony - (epsrel & y)) * (EE * epsC * conf.factorh) ).mapP(.05, clip);

      if(intern_isTeaching && gamma > 0){
        // scale of the additional terms
        const Matrix& metric = (A^T) * Lplus.multTM() * A;

        const Matrix& y      = y_buffer[(t-1)% buffersize];
        const Matrix& xsi    = y_teaching - y;
        const Matrix& delta  = xsi.multrowwise(g_prime);
        C += ((metric * delta*(x^T) ) * (gamma * epsC)).mapP(.05, clip);
        h += ((metric * delta)        * (gamma * epsC * conf.factorh)).mapP(.05, clip);
        // after we applied teaching signal it is switched off until new signal is given
        intern_isTeaching    = false;
      }
    }
}

void set_default_config(){
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
