class Shape
{
    public:
        int x, y;
        float radius, rotation, r, g, b;
        Shape(int xx, int yy, float ra, float ro, float red, float green, float blue) : x(xx), y(yy), radius(ra), rotation(ro), r(red), g(green), b(blue){}
        Shape() {x = 0; y = 0; radius = 0; rotation = 0; r = 0; g = 0; b = 0;}
};
