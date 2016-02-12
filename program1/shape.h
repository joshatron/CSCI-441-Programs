class Shape
{
    public:
        //shapes: 0-square, 1-circles, 2-triangle, 3-line
        int x, y, shape;
        float radius, rotation, r, g, b;
        Shape(int xx, int yy, int s, float ra, float ro, float red, float green, float blue) : x(xx), y(yy), shape(s), radius(ra), rotation(ro), r(red), g(green), b(blue){}
        Shape() {x = 0; y = 0; shape = 0; radius = 0; rotation = 0; r = 0; g = 0; b = 0;}
};
