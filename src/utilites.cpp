#include "utilites.h"

#define E .001

int getOctantOfLine(const int& x0, const int& y0,
                                     const int& x1, const int& y1){
    // Turn into vectors
    Vec3f a(x0, y0, 0);
    Vec3f b(x1, y1, 0);

    // Find Direction of line
    Vec3f c = b - a;
    c.Normalize();

    // Get angle based on x-axis
    double rad = c.AngleBetween(Vec3f(1,0,0) );

    std::cout << "angle " << rad << std::endl;

    int octant;

    if (rad <= M_PI / 4 ){
        octant = 0;

    } else if ( rad <= 2 * M_PI / 4 ){
        octant = 1;

    } else if ( rad <= 3 * M_PI / 4 ){
        octant = 2;

    } else if ( rad <= 4 * M_PI / 4 ){
        octant = 3;

    } else if ( rad <= 5 * M_PI / 4 ){
        octant = 4;

    } else if ( rad <= 6 * M_PI / 4 ){
        octant = 5;

    } else if ( rad <= 7 * M_PI / 4 ){
        octant = 6;

    } else {
        octant = 7;

    }

    return octant;

}

void switchToOctantFromZero(int octant, const int & _x, const int & _y, int & x, int & y){

    int temp;
    x = _x;
    y = _y;

    switch(octant) {
      case 0:
          break;
      case 1:
          temp = x;
          x = y;
          y = temp;
          break;

      case 2:
          temp = x;
          x = -1*y;
          y = temp;
          break;

       case 3:
          x *= -1;
          break;

       case 4:
          x *= -1;
          y *= -1;
          break;

       case 5:
          temp = x;
          x = -1* y;
          y = -1* temp;
          break;

       case 6:
          temp = x;
          x = y;
          y = -1 * temp;
          break;
       case 7:
          y*= -1;
          break;
      default:
          std::cout << "Bad Octant Input\n";
          assert(false);
    }//switch
}

void switchToOctantZeroFrom(int octant, const int & _x, const int & _y, int & x, int & y){

    int temp;
    x = _x;
    y = _y;

    switch(octant) {
      case 0:
          break;
      case 1:
          temp = x;
          x = y;
          y = temp;
          break;

      case 2:
          temp = x;
          x = y;
          y = -1*temp;
          break;
       case 3:
          x *= -1;
          break;
       case 4:
          x *= -1;
          y *= -1;
          break;
       case 5:
          temp = x;
          x = -1* y;
          y = -1* temp;
          break;
       case 6:
          temp = x;
          x = -1* y;
          y = temp;
          break;
       case 7:
          y*= -1;
          break;
      default:
          std::cout << "Bad Octant Input\n";
          assert(false);
    }//switch
}

std::vector<int> bresenham_line_plot(const int& _x0, const int& _y0,
                                     const int& _x1, const int& _y1){

    // This algorithm only works if we are in 0th quad
    // We will convert input to 0th octant
    int octant = getOctantOfLine(_x0,_y0,_x1,_y1);
    int x0,y0,x1,y1;

    switchToOctantZeroFrom(octant, _x0, _y0, x0, y0);
    switchToOctantZeroFrom(octant, _x1, _y1, x1, y1);

    // Where we store results
    std::vector<int> plotRes;

    int dx = x1 - x0;
    int dy = y1 - y0;

    int D = 2 * dy - dx;
    plotRes.push_back(_x0);
    plotRes.push_back(_y0);

    int y = y0;

    for(int x = x0 + 1; x <= x1; x++){

        if (D > 0){

            y = y + 1;

            // Convert before pushing to res
            int rx, ry;
            switchToOctantFromZero(octant,x,y,rx,ry);
            plotRes.push_back(rx);
            plotRes.push_back(ry);

            D = D + ( 2 * dy - 2 * dx);

        }else{

            int rx, ry;
            switchToOctantFromZero(octant,x,y,rx,ry);
            plotRes.push_back(rx);
            plotRes.push_back(ry);
            D = D + ( 2 * dy );

        }

    }//for

    return plotRes;

}//blplot


bool get_line_intersection(const double & p0_x, const double & p0_y, const double & p1_x, const double & p1_y,
                           const double & p2_x, const double & p2_y, const double & p3_x, const double & p3_y,
                           double & i_x, double & i_y){

  // Tricks of the Windows Game Programming Gurus
  double s1_x, s1_y, s2_x, s2_y;

  s1_x = p1_x - p0_x;     s1_y = p1_y - p0_y;
  s2_x = p3_x - p2_x;     s2_y = p3_y - p2_y;

  double s, t;
  s = (-s1_y * (p0_x - p2_x) + s1_x * (p0_y - p2_y)) / (-s2_x * s1_y + s1_x * s2_y);
  t = ( s2_x * (p0_y - p2_y) - s2_y * (p0_x - p2_x)) / (-s2_x * s1_y + s1_x * s2_y);

  if (s >= 0 && s <= 1 && t >= 0 && t <= 1)
  {
      // Collision detected
      i_x = p0_x + (t * s1_x);
      i_y = p0_y + (t * s1_y);
      return true;
  }

    return false; // No collision
}

