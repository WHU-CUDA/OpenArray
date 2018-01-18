#ifndef __UTILS_HPP__
#define __UTILS_HPP__

#include <iostream>
#include <math.h>
#include <mpi.h>
#include <algorithm>
#include <type_traits>
#include <boost/format.hpp>
#include "../common.hpp"
#include <armadillo>

extern "C" {
  void tic(const char* s);
  void toc(const char* s);
  void show_time(const char* s);
  void show_all();
}

namespace oa {
  namespace utils {
    template<class T>
    void print_data_t(T* buf, const Shape& shape) {
      const int M = shape[0];
      const int N = shape[1];
      const int P = shape[2];
      const int buf_size = M * N * P;

      std::string frt_str;
      if (std::is_same<T, int>::value) {
        frt_str = "%11d";
      } else if (std::is_same<T, float>::value
        || std::is_same<T, double>::value) {
        frt_str = "%20.15f";

      } else if (std::is_same<T, bool>::value) {
        frt_str = "%4d";
      }

      static auto abs_compare = [](T a, T b) {
        return std::abs(a) < std::abs(b);
      };

      T val = 1;

      if (std::is_same<T, float>::value ||
        std::is_same<T, double>::value) {
        T max = *std::max_element(buf, buf+buf_size, abs_compare);

        if(std::abs(max) > 0.1){

          int factor = (int)log10(std::abs(max));

          if (factor > 2) {
            std::cout<< " * 1E"<<factor<<std::endl;
            val  = val / pow(10, factor);
          }
        } else if (std::abs(max) < 0.001 && std::abs(max) > 0) {

          int factor = (int)log10(std::abs(max));

          if (factor <= -3) {
            std::cout<< " * 1E"<<factor<<std::endl;
            val  = val / pow(10, factor);
          }
        }

        //std::cout<<"min = "<< max << std::endl;
        //std::cout<<"factor = "<<factor << std::endl;
      }

      //for (int i = 0; i < 24; i++) std::cout<<buf[i]<<" "<<std::endl;

      for(int k = 0; k < P; ++k) {
        std::cout<<"[k = " << k << "]" << std::endl;
        for(int i = 0; i < M; ++i) {
          for(int j = 0; j < N; ++j) {
            std::cout<<boost::format(frt_str) % (buf[i + j * M + k * M * N] * val);
          }
          std::cout<<std::endl;
        }
      }
    }

    template <typename T>
    DataType to_type() {
      if (std::is_same<T, int>::value) return DATA_INT;
      if (std::is_same<T, float>::value) return DATA_FLOAT;
      if (std::is_same<T, double>::value) return DATA_DOUBLE;
      return DATA_UNKNOWN;
    }

    template<class T>
    arma::Cube<T> make_cube(const Shape& shape, void* buf = NULL) {
      if (buf != NULL) {
        return arma::Cube<T>((T*)buf, shape[0], shape[1], shape[2],
          false, false);	
      }
      return arma::Cube<T>(shape[0], shape[1], shape[2]);
    };

    template<class T>
    arma::Col<T> make_vec(int size, T* buf = NULL) {
      if (buf != NULL) {
        return arma::Col<T>((T*)buf, size, false, false);	
      }
      return arma::Col<T>(size);
    };
    
    template<class T>
    struct dtype {
      const static DataType type = -1;
      const static int size = -1;
      static MPI_Datatype mpi_type() {
        if (std::is_same<T, int>::value) {
          return MPI_INT;
        } else if (std::is_same<T, float>::value) {
          return MPI_FLOAT;
        } else if (std::is_same<T, double>::value) {
          return MPI_DOUBLE;
        } else {
          return MPI_DATATYPE_NULL;
        }
      }
    };

    template<>
    struct dtype<bool>{
      const static DataType type = DATA_BOOL;
      const static int size = sizeof(bool);
    };

    template<>
    struct dtype<int>{
      const static DataType type = DATA_INT;
      const static int size = sizeof(int);
    };

    template<>
    struct dtype<float>{
      const static DataType type = DATA_FLOAT;
      const static int size = sizeof(float);
    };

    template<>
    struct dtype<double>{
      const static DataType type = DATA_DOUBLE;
      const static int size = sizeof(double);
    };

    DataType cast_data_type(DataType t1, DataType t2);

    //! display array for a buffer. 
    void print_data(void* buf, const Shape& shape, DATA_TYPE dt);

    int data_size(int data_type);
    
    MPI_Datatype mpi_datatype(int t);
    
    bool is_equal_shape(const Shape& u, const Shape& v);

    //convert a given datatype to a string
    std::string get_type_string(DataType t);

    int get_shape_dimension(Shape S);

    bool check_legal_shape_calc(Shape u, Shape v);


  }  
}

#endif
