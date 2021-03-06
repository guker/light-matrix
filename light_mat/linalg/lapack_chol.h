/**
 * @file lapack_chol.h
 *
 * @brief Cholesky factorization
 *
 * @author Dahua Lin
 */


#ifdef _MSC_VER
#pragma once
#endif

#ifndef LIGHTMAT_LAPACK_CHOL_H_
#define LIGHTMAT_LAPACK_CHOL_H_

#include <light_mat/linalg/lapack_fwd.h>
#include <light_mat/math/math.h>

/************************************************
 *
 *  external LAPACK functions
 *
 ************************************************/

extern "C"
{
	void LMAT_LAPACK_NAME(spoequ)( const lapack_int* n, const float* a, const lapack_int* lda, float* s,
	             float* scond, float* amax, lapack_int* info );

	void LMAT_LAPACK_NAME(dpoequ)( const lapack_int* n, const double* a, const lapack_int* lda, double* s,
	             double* scond, double* amax, lapack_int* info );


	void LMAT_LAPACK_NAME(spotrf)( const char* uplo, const lapack_int* n, float* a, const lapack_int* lda,
	             lapack_int* info );

	void LMAT_LAPACK_NAME(spocon)( const char* uplo, const lapack_int* n, const float* a,
	             const lapack_int* lda, const float* anorm, float* rcond, float* work,
	             lapack_int* iwork, lapack_int* info );

	void LMAT_LAPACK_NAME(spotri)( const char* uplo, const lapack_int* n, float* a, const lapack_int* lda,
	             lapack_int* info );

	void LMAT_LAPACK_NAME(spotrs)( const char* uplo, const lapack_int* n, const lapack_int* nrhs,
	             const float* a, const lapack_int* lda, float* b, const lapack_int* ldb,
	             lapack_int* info );

	void LMAT_LAPACK_NAME(sporfs)( const char* uplo, const lapack_int* n, const lapack_int* nrhs,
	             const float* a, const lapack_int* lda, const float* af,
	             const lapack_int* ldaf, const float* b, const lapack_int* ldb, float* x,
	             const lapack_int* ldx, float* ferr, float* berr, float* work,
	             lapack_int* iwork, lapack_int* info );

	void LMAT_LAPACK_NAME(dpotrf)( const char* uplo, const lapack_int* n, double* a, const lapack_int* lda,
	             lapack_int* info );

	void LMAT_LAPACK_NAME(dpocon)( const char* uplo, const lapack_int* n, const double* a,
	             const lapack_int* lda, const double* anorm, double* rcond,
	             double* work, lapack_int* iwork, lapack_int* info );

	void LMAT_LAPACK_NAME(dpotri)( const char* uplo, const lapack_int* n, double* a, const lapack_int* lda,
	             lapack_int* info );

	void LMAT_LAPACK_NAME(dpotrs)( const char* uplo, const lapack_int* n, const lapack_int* nrhs,
	             const double* a, const lapack_int* lda, double* b,
	             const lapack_int* ldb, lapack_int* info );

	void LMAT_LAPACK_NAME(dporfs)( const char* uplo, const lapack_int* n, const lapack_int* nrhs,
	             const double* a, const lapack_int* lda, const double* af,
	             const lapack_int* ldaf, const double* b, const lapack_int* ldb,
	             double* x, const lapack_int* ldx, double* ferr, double* berr,
	             double* work, lapack_int* iwork, lapack_int* info );

	void LMAT_LAPACK_NAME(sposv)( const char* uplo, const lapack_int* n, const lapack_int* nrhs, float* a,
	            const lapack_int* lda, float* b, const lapack_int* ldb, lapack_int* info );

	void LMAT_LAPACK_NAME(sposvx)( const char* fact, const char* uplo, const lapack_int* n,
	             const lapack_int* nrhs, float* a, const lapack_int* lda, float* af,
	             const lapack_int* ldaf, char* equed, float* s, float* b,
	             const lapack_int* ldb, float* x, const lapack_int* ldx, float* rcond,
	             float* ferr, float* berr, float* work, lapack_int* iwork,
	             lapack_int* info );

	void LMAT_LAPACK_NAME(dposv)( const char* uplo, const lapack_int* n, const lapack_int* nrhs, double* a,
	            const lapack_int* lda, double* b, const lapack_int* ldb, lapack_int* info );

	void LMAT_LAPACK_NAME(dposvx)( const char* fact, const char* uplo, const lapack_int* n,
	             const lapack_int* nrhs, double* a, const lapack_int* lda, double* af,
	             const lapack_int* ldaf, char* equed, double* s, double* b,
	             const lapack_int* ldb, double* x, const lapack_int* ldx, double* rcond,
	             double* ferr, double* berr, double* work, lapack_int* iwork,
	             lapack_int* info );
}


namespace lmat { namespace lapack {

	// forward declarations

	template<typename T> class chol_fac;


	/************************************************
	 *
	 *  Cholesky classes
	 *
	 ************************************************/

	namespace internal
	{
		LMAT_ENSURE_INLINE
		inline char check_chol_uplo(char c)
		{
			char r;
			if (c == 'U' || c == 'u') r = 'U';
			else if (c == 'L' || c == 'l') r = 'L';
			else
				throw invalid_argument("Invalid value for uplo");

			return r;
		}
	}


	template<typename T>
	class chol_base
	{
	public:
		explicit chol_base( char uplo )
		: m_uplo(internal::check_chol_uplo(uplo)), m_dim(0), m_a() { }

		bool empty() const
		{
			return m_dim == 0;
		}

		bool is_lower() const
		{
			return m_uplo == 'L';
		}

		bool is_upper() const
		{
			return m_uplo == 'U';
		}

		index_t dim() const
		{
			return m_dim;
		}

		const dense_matrix<T>& intern() const
		{
			return m_a;
		}

		template<class L>
		void get(IRegularMatrix<L, T>& mat) const
		{
			mat.require_size(m_dim, m_dim);
			zero(mat);

			if (is_lower())
			{
				copy_tril(m_a, mat);
			}
			else
			{
				copy_triu(m_a, mat);
			}
		}

	protected:
		template<class Mat>
		void set_mat(const IMatrixXpr<Mat, T>& mat)
		{
			LMAT_CHECK_DIMS( mat.nrows() == mat.ncolumns() )

			m_dim = mat.nrows();
			m_a = mat;
		}

	protected:
		const char m_uplo;
		index_t m_dim;
		dense_matrix<T> m_a;
	};


	template<>
	class chol_fac<float> : public chol_base<float>
	{
	public:
		explicit chol_fac(char uplo='L')
		: chol_base<float>(uplo) { }

		template<class Mat>
		explicit chol_fac(const IMatrixXpr<Mat, float>& a, char uplo='L')
		: chol_base<float>(uplo)
		{
			set(a);
		}

		template<class Mat>
		void set(const IMatrixXpr<Mat, float>& mat)
		{
			this->set_mat(mat);
			trf(this->m_a, this->m_uplo);
		}

		template<class B>
		void solve_inplace(IRegularMatrix<B, float>& b) const
		{
			LMAT_CHECK_PERCOL_CONT(B)

			lapack_int n = (lapack_int)(this->m_dim);
			lapack_int nrhs = (lapack_int)(b.ncolumns());
			lapack_int lda = (lapack_int)(this->m_a.col_stride());
			lapack_int ldb = (lapack_int)(b.col_stride());
			lapack_int info = 0;

			LMAT_CALL_LAPACK(spotrs, (&(this->m_uplo), &n, &nrhs,
					this->m_a.ptr_data(), &lda, b.ptr_data(), &ldb, &info));
		}

		template<class B, class X>
		void solve(const IMatrixXpr<B, float>& b, IRegularMatrix<X, float>& x) const
		{
			LMAT_CHECK_PERCOL_CONT(X)

			x.derived() = b.derived();
			solve_inplace(x);
		}

		float eval_det() const   // det of the factor
		{
			const dense_matrix<float> &a_ = this->m_a;
			const index_t n = a_.nrows();
			double r(1.0);
			for (index_t i = 0; i < n; ++i) r *= a_(i,i);
			return static_cast<float>(r);
		}

		float eval_logdet() const  // logdet of the factor
		{
			const dense_matrix<float> &a_ = this->m_a;
			const index_t n = a_.nrows();
			double r(0.0);
			for (index_t i = 0; i < n; ++i) r += math::log(a_(i,i));
			return static_cast<float>(r);
		}

	public:
		template<class A>
		static void inv_inplace(IRegularMatrix<A, float>& a, char uplo='L')
		{
			LMAT_CHECK_PERCOL_CONT(A)
			LMAT_CHECK_DIMS( a.nrows() == a.ncolumns() );

			uplo = internal::check_chol_uplo(uplo);
			trf(a, uplo);

			lapack_int n = (lapack_int)a.nrows();
			lapack_int lda = (lapack_int)a.col_stride();
			lapack_int info = 0;

			LMAT_CALL_LAPACK(spotri, (&uplo, &n, a.ptr_data(), &lda, &info));

			lmat::internal::complete_sym(a.nrows(), a, uplo);
		}

		template<class A, class B>
		static void inv(const IMatrixXpr<A, float>& a, IRegularMatrix<B, float>& b, char uplo='L')
		{
			LMAT_CHECK_PERCOL_CONT(B)

			b.derived() = a.derived();
			inv_inplace(b, uplo);
		}

	private:

		template<class A>
		static void trf(IRegularMatrix<A, float>& a, char uplo)
		{
			lapack_int n = (lapack_int)(a.nrows());
			lapack_int lda = (lapack_int)(a.col_stride());
			lapack_int info = 0;

			LMAT_CALL_LAPACK(spotrf, (&uplo, &n, a.ptr_data(), &lda, &info));
		}
	};


	template<>
	class chol_fac<double> : public chol_base<double>
	{
	public:
		explicit chol_fac(char uplo='L')
		: chol_base<double>(uplo) { }

		template<class Mat>
		explicit chol_fac(const IMatrixXpr<Mat, double>& a, char uplo='L')
		: chol_base<double>(uplo)
		{
			set(a);
		}

		template<class Mat>
		void set(const IMatrixXpr<Mat, double>& mat)
		{
			this->set_mat(mat);
			trf(this->m_a, this->m_uplo);
		}

		template<class B>
		void solve_inplace(IRegularMatrix<B, double>& b) const
		{
			LMAT_CHECK_PERCOL_CONT(B)

			lapack_int n = (lapack_int)(this->m_dim);
			lapack_int nrhs = (lapack_int)(b.ncolumns());
			lapack_int lda = (lapack_int)(this->m_a.col_stride());
			lapack_int ldb = (lapack_int)(b.col_stride());
			lapack_int info = 0;

			LMAT_CALL_LAPACK(dpotrs, (&(this->m_uplo), &n, &nrhs,
					this->m_a.ptr_data(), &lda, b.ptr_data(), &ldb, &info));
		}

		template<class B, class X>
		void solve(const IMatrixXpr<B, double>& b, IRegularMatrix<X, double>& x) const
		{
			LMAT_CHECK_PERCOL_CONT(X)

			x.derived() = b.derived();
			solve_inplace(x);
		}


		double eval_det() const   // det of the factor
		{
			const dense_matrix<double> &a_ = this->m_a;
			const index_t n = a_.nrows();
			double r(1.0);
			for (index_t i = 0; i < n; ++i) r *= a_(i,i);
			return r;
		}

		double eval_logdet() const  // logdet of the factor
		{
			const dense_matrix<double> &a_ = this->m_a;
			const index_t n = a_.nrows();
			double r(0.0);
			for (index_t i = 0; i < n; ++i) r += math::log(a_(i,i));
			return r;
		}

	public:
		template<class A>
		static void inv_inplace(IRegularMatrix<A, double>& a, char uplo='L')
		{
			LMAT_CHECK_PERCOL_CONT(A)
			LMAT_CHECK_DIMS( a.nrows() == a.ncolumns() );

			uplo = internal::check_chol_uplo(uplo);
			trf(a, uplo);

			lapack_int n = (lapack_int)a.nrows();
			lapack_int lda = (lapack_int)a.col_stride();
			lapack_int info = 0;

			LMAT_CALL_LAPACK(dpotri, (&uplo, &n, a.ptr_data(), &lda, &info));

			lmat::internal::complete_sym(a.nrows(), a, uplo);
		}

		template<class A, class B>
		static void inv(const IMatrixXpr<A, double>& a, IRegularMatrix<B, double>& b, char uplo='L')
		{
			LMAT_CHECK_PERCOL_CONT(B)

			b.derived() = a.derived();
			inv_inplace(b, uplo);
		}

	private:

		template<class A>
		static void trf(IRegularMatrix<A, double>& a, char uplo)
		{
			lapack_int n = (lapack_int)(a.nrows());
			lapack_int lda = (lapack_int)(a.col_stride());
			lapack_int info = 0;

			LMAT_CALL_LAPACK(dpotrf, (&uplo, &n, a.ptr_data(), &lda, &info));
		}
	};


	/************************************************
	 *
	 *  POSV
	 *
	 ************************************************/

	template<class A, class B>
	inline void posv(IRegularMatrix<A, float>& a, IRegularMatrix<B, float>& b, char uplo='L')
	{
		LMAT_CHECK_PERCOL_CONT(A)
		LMAT_CHECK_PERCOL_CONT(B)

		uplo = internal::check_chol_uplo(uplo);
		LMAT_CHECK_DIMS( a.nrows() == a.ncolumns() && a.nrows() == b.nrows() );

		lapack_int n = (lapack_int)a.nrows();
		lapack_int nrhs = (lapack_int)b.ncolumns();
		lapack_int lda = (lapack_int)a.col_stride();
		lapack_int ldb = (lapack_int)b.col_stride();

		lapack_int info = 0;
		LMAT_CALL_LAPACK(sposv, (&uplo, &n, &nrhs, a.ptr_data(), &lda, b.ptr_data(), &ldb, &info));
	}

	template<class A, class B>
	inline void posv(IRegularMatrix<A, double>& a, IRegularMatrix<B, double>& b, char uplo='L')
	{
		LMAT_CHECK_PERCOL_CONT(A)
		LMAT_CHECK_PERCOL_CONT(B)

		uplo = internal::check_chol_uplo(uplo);
		LMAT_CHECK_DIMS( a.nrows() == a.ncolumns() && a.nrows() == b.nrows() );

		lapack_int n = (lapack_int)a.nrows();
		lapack_int nrhs = (lapack_int)b.ncolumns();
		lapack_int lda = (lapack_int)a.col_stride();
		lapack_int ldb = (lapack_int)b.col_stride();

		lapack_int info = 0;
		LMAT_CALL_LAPACK(dposv, (&uplo, &n, &nrhs, a.ptr_data(), &lda, b.ptr_data(), &ldb, &info));
	}


} }


namespace lmat
{

	template<class Arg> class pdinv_expr;

	/************************************************
	 *
	 *  inversion expression
	 *
	 ************************************************/

	template<class Arg>
	struct matrix_traits<pdinv_expr<Arg> >
	: public matrix_xpr_traits_base<
	  typename meta::value_type_of<Arg>::type,
	  meta::sq_dim<Arg>::value,
	  meta::sq_dim<Arg>::value,
	  typename meta::domain_of<Arg>::type> { };

	template<class Arg>
	class pdinv_expr
	: public matrix_xpr_base<pdinv_expr<Arg> >
	{
		typedef matrix_xpr_base<pdinv_expr<Arg> > base_t;

	public:
		LMAT_ENSURE_INLINE
		explicit pdinv_expr(const Arg& a, char uplo_)
		: base_t(a.nrows(), a.ncolumns())
		, m_uplo(lapack::internal::check_chol_uplo(uplo_))
		, m_arg(a)
		{
			LMAT_CHECK_DIMS( a.nrows() == a.ncolumns() )
		}

		LMAT_ENSURE_INLINE
		char uplo() const
		{
			return m_uplo;
		}

		LMAT_ENSURE_INLINE
		const Arg& arg() const
		{
			return m_arg;
		}

	private:
		const char m_uplo;
		const Arg& m_arg;
	};


	template<class Arg, class DMat>
	inline void evaluate(const pdinv_expr<Arg>& expr,
			IRegularMatrix<DMat, typename matrix_traits<Arg>::value_type>& dmat)
	{
		LMAT_CHECK_PERCOL_CONT(DMat)

		typedef typename matrix_traits<Arg>::value_type T;
		lapack::chol_fac<T>::inv(expr.arg(), dmat.derived(), expr.uplo());
	}

	template<class Arg>
	LMAT_ENSURE_INLINE
	inline pdinv_expr<Arg> pdinv(const IMatrixXpr<Arg, float>& a, const char uplo='L')
	{
		return pdinv_expr<Arg>(a.derived(), uplo);
	}

	template<class Arg>
	LMAT_ENSURE_INLINE
	inline pdinv_expr<Arg> pdinv(const IMatrixXpr<Arg, double>& a, const char uplo='L')
	{
		return pdinv_expr<Arg>(a.derived(), uplo);
	}


	template<typename T, class Mat>
	LMAT_ENSURE_INLINE
	inline T pddet2(const IRegularMatrix<Mat, T>& a,
			const index_t i0, const index_t i1, const index_t j0, const index_t j1)
	{
		const Mat& a_ = a.derived();
		return a_(i0, j0) * a_(i1, j1) - a_(i1, j0) * a_(i0, j1);
	}

	template<typename T, class Mat>
	LMAT_ENSURE_INLINE
	inline T pddet2(const IRegularMatrix<Mat, T>& a)
	{
		const Mat& a_ = a.derived();
		return a_(0, 0) * a_(1, 1) - math::sqr(a_(1, 0));
	}

	template<typename T, class Mat>
	LMAT_ENSURE_INLINE
	inline T pddet3(const IRegularMatrix<Mat, T>& a)
	{
		const Mat& a_ = a.derived();
		double t0 = double(pddet2(a, 0, 1, 0, 1)) * a_(2, 2);
		double t1 = double(pddet2(a, 0, 2, 0, 1)) * a_(1, 2);
		double t2 = double(pddet2(a, 1, 2, 0, 1)) * a_(0, 2);
		return static_cast<T>(t0 - t1 + t2);
	}

	template<typename T, class Arg>
	inline T pddet(const IRegularMatrix<Arg, T>& a)
	{
		LMAT_CHECK_DIMS( is_square(a) )
		const index_t n = a.nrows();

		if (n == 1)
		{
			return a(0, 0);
		}
		else if (n == 2)
		{
			return pddet2(a);
		}
		else if (n == 3)
		{
			return pddet3(a);
		}
		else
		{
			lapack::chol_fac<T> chol(a);
			return math::sqr(chol.eval_det());
		}
	}


	template<typename T, class Arg>
	inline T pdlogdet(const IRegularMatrix<Arg, T>& a)
	{
		LMAT_CHECK_DIMS( is_square(a) )

		const index_t n = a.nrows();
		if (n == 1)
		{
			return math::log(a(0, 0));
		}
		else if (n == 2)
		{
			return math::log(pddet2(a));
		}
		else if (n == 3)
		{
			return math::log(pddet3(a));
		}
		else
		{
			lapack::chol_fac<T> chol(a);
			return chol.eval_logdet() * T(2);
		}
	}


}



#endif /* LAPACK_CHOL_H_ */
