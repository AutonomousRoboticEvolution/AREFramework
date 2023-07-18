#include "homeokinesis_controller.hpp"


void Homeokinesis::init(int nb_inputs, int nb_outputs){

}

std::vector<double> Homeokinesis::update(const std::vector<double> &sensorValues){

}

void learn(){
    using Matrix = Eigen::MatrixXd;

    // the effective x/y is (actual-steps4delay) element of buffer
    int s4delay = ::clip(conf.steps4Delay,1,buffersize-1);
    const Matrix& x       = x_buffer[(t - max(s4delay,1) + buffersize) % buffersize];
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
