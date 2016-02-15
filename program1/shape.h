class Shape
{
    public:
        //shapes: 0-square, 1-circles, 2-triangle, 3-line
        int x, y, sides;
        float radius, r, g, b;
        Shape(int xx, int yy, int s, float ra, float red, float green, float blue) : x(xx), y(yy), sides(s), radius(ra), r(red), g(green), b(blue){}
        Shape() {x = 0; y = 0; sides = 4; radius = 30; r = 1; g = 1; b = 1;}
};
