#include <cassert>


typedef struct
{
    int height;
    int width;
    float* array;
} Matrix;

struct SimpleMatrix
{
    Matrix data;
    SimpleMatrix(const SimpleMatrix& copy);
    SimpleMatrix(SimpleMatrix&& move);
    SimpleMatrix& operator=(const SimpleMatrix& copy);
    SimpleMatrix& operator=(SimpleMatrix&& move);//seems need?!
    ~SimpleMatrix();
    SimpleMatrix(int w, int h);
    SimpleMatrix(char* FileName);
    float& operator()(int i, int j);
    const float& operator()(int i, int j) const;

    float& operator()(int i);
    const float& operator()(int i) const;
    
    void dump();
    void dump_plane();

    int getHeight()const // rewrite
    {
        return data.height;    
    }
    int getWidth() const// rewrite..
    {
        return data.width;    
    }

    friend bool check_shapes(const SimpleMatrix& lhs, const SimpleMatrix& rhs)
    {
        return (lhs.data.width == rhs.data.width && 
                    lhs.data.height == rhs.data.height);    
    }
    friend bool check_multiply_shapes(const SimpleMatrix& res, const
    SimpleMatrix& lhs, const SimpleMatrix& rhs)
    {
        return (lhs.data.height == rhs.data.width &&
                lhs.data.width == rhs.data.height &&
                res.data.width == rhs.data.width  &&
                res.data.height == lhs.data.height);    
    }


    void reshape(int new_w, int new_h); 
    private:
    void copy_elements(const SimpleMatrix& rhs);   
};
