#include <iostream>
#include <vector>
#include <cmath>
#include <complex>
#include <chrono>
#include <fftw3.h>

using namespace std;

class Fft {
public:
    virtual vector<complex<double>> calculate(const vector<complex<double>>&) = 0;
    virtual std::string name() = 0;
    virtual ~Fft() = default;
};

class LibFft : public Fft{
public:
    virtual vector<complex<double>> calculate(const vector<complex<double>> &a);
    virtual std::string name();
};

class MyFft : public Fft {
public:
    virtual vector<complex<double>> calculate(const vector<complex<double>> &a);
    virtual std::string name();

private:
    void fft(vector<complex<double>>& a);
};

vector<complex<double>> LibFft::calculate(const vector<complex<double>>& a) {
    fftw_complex* in = new fftw_complex[a.size()];
    fftw_complex* out = new fftw_complex[a.size()];
    fftw_plan p, q;
    for (size_t i = 0; i < a.size(); i++) {
      in[i][0] = a[i].real();
      in[i][1] = a[i].imag();
    }
    p = fftw_plan_dft_1d(a.size(), in, out, FFTW_BACKWARD, FFTW_ESTIMATE);
    fftw_execute(p);
    fftw_destroy_plan(p);
    vector<complex<double>> b(a.size());
    for (size_t i = 0; i < a.size(); i++) {
      b[i] = complex<double>(out[i][0], out[i][1]);
    }
    delete[] in;
    delete[] out;

    return b;
}

std::string LibFft::name() {
    return std::string("fftw3");
}

std::string MyFft::name() {
    return std::string("my own fft implementation");
}

vector<complex<double>> MyFft::calculate(const vector<complex<double>>& a) {
    vector<complex<double>> b(a);
    fft(b);
    return b;
}

void MyFft::fft(vector<complex<double>>& a) {
	if (a.size() < 2) {
        return;
    }
  	vector<complex<double>> a0(a.size() / 2), a1(a.size() / 2);
  	for (size_t i = 0; i < a.size() / 2; i++) {
  	  	a0[i] = a[2 * i];
  	  	a1[i] = a[2 * i + 1];
  	}
  	calculate(a0);
  	calculate(a1);
  	double ang = 2 * M_PI / a.size();
  	complex<double> w(1), wn(cos(ang), sin(ang));
  	for (size_t i = 0; i < a.size() / 2; i++) {
  		  a[i] = a0[i] + w * a1[i];
  		  a[i + a.size() / 2] = a0[i] - w * a1[i];
  		  w *= wn;
  	}
}

int main() {
    vector<Fft*> implementations{ new LibFft(), new MyFft()};

    for (size_t pow = 15; pow <= 25; pow++) {
        vector<complex<double>> v;
        for (int i = 1; i <= (1 << pow); i++) {
            v.push_back(i);
        }

        std::cout << "Running FFT for N:" << pow << " size:" << (1<<pow) << std::endl;
        for (auto& impl : implementations) {
            vector<complex<double>> t(v);
            auto start = std::chrono::high_resolution_clock::now();
            impl->calculate(t);
            auto end = std::chrono::high_resolution_clock::now();
            chrono::duration<double, std::milli> elapsed = end - start;

            std::cout << "\timplementation:" << impl->name() << " elapsed:" << elapsed.count() << " ms." << std::endl;
        }
    }

    for (auto i : implementations) {
        delete i;
    }
    return 0;
}