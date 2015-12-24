#ifndef RANDOMGENERATOR_INC
#define RANDOMGENERATOR_INC

/** RandomGenerator class which contains NO static random methods.
    
    These methods are based on a uniform random generator,
    named "Mersenne Twister", available at:
    http://www.math.keio.ac.jp/matumoto/emt.html.
    Copyright Makoto Matsumoto and Takuji Nishimura.
    (Have a look inside the implementation file for details).

    The random generator can be initialized with the manualSeed() method.
    Otherwise, it will be automatically initialized with a seed based
    on the current computer clock.
*/

namespace TLUtil
{

class RandomGenerator
{
  public:

    // Internal variables for the Mersenne Twister generator
     static const int n = 624;
     static const int m = 397;
     unsigned long *state; /* the array for the state vector  */
     int left;
     int initf;
     unsigned long *next;

    // Internal variables for the normal distribution generator
     float normal_x;
     float normal_y;
     float normal_rho;
     bool normal_is_valid;

     RandomGenerator();
     RandomGenerator(unsigned long s);

    // Internal method for the Mersenne Twister generator
     void nextState();

    /// Initializes the random number generator with the computer clock.
     void seed();

    /// Initializes the random number generator with the given long "s".
     void manualSeed(unsigned long s);

    /// Generates a uniform 32 bits integer.
     unsigned long random();

    /// Generates a uniform random number on [0,1[.
     float uniform();

    /// Returns in #indices# #n_indices# shuffled. (between 0 and #n_indices-1#).
     void getShuffledIndices(int *indices, int n_indices);

    /// Shuffles tabular, which contains #n_elems# of size #size_elem#.
     void shuffle(void *tabular, int size_elem, int n_elems);

    /// Generates a uniform random number on [a,b[ (b>a).
     float boundedUniform(float a, float b);

    /** Generates a random number from a normal distribution.
        (With mean #mean# and standard deviation #stdv >= 0#).
    */
     float normal(float mean=0, float stdv=1);

    /** Generates a random number from an exponential distribution.
        The density is $p(x) = lambda * exp(-lambda * x)$, where
        lambda is a positive number.
    */
     float exponential(float lambda);

    /** Returns a random number from a Cauchy distribution.
        The Cauchy density is $p(x) = sigma/(pi*(sigma^2 + (x-median)^2))$
    */
     float cauchy(float median=0, float sigma=1);

    /** Generates a random number from a log-normal distribution.
        (#mean > 0# is the mean of the log-normal distribution
        and #stdv# is its standard deviation).
    */
     float logNormal(float mean, float stdv);

    /** Generates a random number from a geometric distribution.
        It returns an integer #i#, where $p(i) = (1-p) * p^(i-1)$.
        p must satisfy $0 < p < 1$.
    */
     int geometric(float p);

    /// Returns true with probability $p$ and false with probability $1-p$ (p > 0).
     bool bernouilli(float p=0.5);

     int fromHistogram(float* cum_hist, int nb_values);


     inline void set(){}
     inline void release(){}
     inline float gaussian_01(){ return normal();}


     ~RandomGenerator();
};

}

#endif
