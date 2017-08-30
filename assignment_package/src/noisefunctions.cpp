#include "noisefunctions.h"

glm::vec3 fract(glm::vec3 p)
{
    glm::vec3 _p(p.x - std::floor(p.x),
                 p.y - std::floor(p.y),
                 p.z - std::floor(p.z));

    return _p;
}

float fract(float n)
{
    return n - std::floor(n);
}

float hash(glm::vec3 p)
{
    p = fract(p * 0.318099f + 0.1f);
    p *= 17.f;

    return fract(p.x * p.y * p.z * (p.x + p.y + p.z));
}

float smoothNoise(glm::vec3 p)
{
    glm::vec3 p1 = glm::vec3(p.x - 1.0, p.y + 1.0,p.z + 1.0);
    glm::vec3 p2 = glm::vec3(p.x,       p.y + 1.0,p.z + 1.0);
    glm::vec3 p3 = glm::vec3(p.x + 1.0, p.y + 1.0,p.z + 1.0);
    glm::vec3 p4 = glm::vec3(p.x - 1.0, p.y,      p.z + 1.0);
    glm::vec3 p5 = glm::vec3(p.x,       p.y,      p.z + 1.0);
    glm::vec3 p6 = glm::vec3(p.x + 1.0, p.y,      p.z + 1.0);
    glm::vec3 p7 = glm::vec3(p.x - 1.0, p.y - 1.0,p.z + 1.0);
    glm::vec3 p8 = glm::vec3(p.x,       p.y - 1.0,p.z + 1.0);
    glm::vec3 p9 = glm::vec3(p.x + 1.0, p.y - 1.0,p.z + 1.0);

    glm::vec3 p10 = glm::vec3(p.x - 1.0,p.y + 1.0,p.z);
    glm::vec3 p11 = glm::vec3(p.x,      p.y + 1.0,p.z);
    glm::vec3 p12 = glm::vec3(p.x + 1.0,p.y + 1.0,p.z);
    glm::vec3 p13 = glm::vec3(p.x - 1.0,p.y,      p.z);
    glm::vec3 p14 = glm::vec3(p.x + 1.0,p.y,      p.z);
    glm::vec3 p15 = glm::vec3(p.x - 1.0,p.y - 1.0,p.z);
    glm::vec3 p16 = glm::vec3(p.x,      p.y - 1.0,p.z);
    glm::vec3 p17 = glm::vec3(p.x + 1.0,p.y - 1.0,p.z);

    glm::vec3 p18 = glm::vec3(p.x - 1.0,p.y + 1.0,p.z - 1.0);
    glm::vec3 p19 = glm::vec3(p.x,p.y + 1.0,p.z - 1.0);
    glm::vec3 p20 = glm::vec3(p.x + 1.0,p.y + 1.0,p.z - 1.0);
    glm::vec3 p21 = glm::vec3(p.x - 1.0,p.y,      p.z - 1.0);
    glm::vec3 p22 = glm::vec3(p.x,p.y,      p.z - 1.0);
    glm::vec3 p23 = glm::vec3(p.x + 1.0,p.y,      p.z - 1.0);
    glm::vec3 p24 = glm::vec3(p.x - 1.0,p.y - 1.0,p.z - 1.0);
    glm::vec3 p25 = glm::vec3(p.x,p.y - 1.0,p.z - 1.0);
    glm::vec3 p26 = glm::vec3(p.x + 1.0,p.y - 1.0,p.z - 1.0);

    float influence1 = 4.0/100.0;
    float influence2 = 1.8/100.0;
    float influence3 = 40.0/100.0;
    //make sure 6*influnce1 + 20*influence2=1

    float n1 =  influence2 * hash(p1);
    float n2 =  influence2 * hash(p2);
    float n3 =  influence2 * hash(p3);
    float n4 =  influence2 * hash(p4);
    float n5 =  influence1 * hash(p5);
    float n6 =  influence2 * hash(p6);
    float n7 =  influence2 * hash(p7);
    float n8 =  influence2 * hash(p8);
    float n9 =  influence2 * hash(p9);

    float n10 = influence2 * hash(p10);
    float n11 = influence1 * hash(p11);
    float n12 = influence2 * hash(p12);
    float n13 = influence1 * hash(p13);
    float n14 = influence3 * hash(p13);
    float n15 = influence1 * hash(p14);
    float n16 = influence2 * hash(p15);
    float n17 = influence1 * hash(p16);
    float n18 = influence2 * hash(p17);

    float n19 = influence2 * hash(p18);
    float n20 = influence2 * hash(p19);
    float n21 = influence2 * hash(p20);
    float n22 = influence2 * hash(p21);
    float n23 = influence1 * hash(p22);
    float n24 = influence2 * hash(p23);
    float n25 = influence2 * hash(p24);
    float n26 = influence2 * hash(p25);
    float n27 = influence2 * hash(p26);

    float average = n1 + n2 +n3 + n4 + n5 + n6 +n7 + n8 + n9 + n10 +n11 + n12 + n13 +
                  n14 + n15 + n16 + n17 + n18 +n19 + n20 + n21 + n22 +n23 + n24 + n25 + n26 +n27;

    return average;
}

// a - lower bound
// b - upper bound
float cosInterp(float a, float b, float t)
{
    float ft = t * Pi;
    float f = (1.f - glm::cos(ft)) * 0.5f;

    return a * (1.f - f) + b * f;
}

float cosNoise(glm::vec3 p)
{
    float x = p.x;
    float y = p.y;
    float z = p.z;

    glm::vec3 p1 = glm::vec3(floor(x), floor(y), floor(z));
    glm::vec3 p2 = glm::vec3(floor(x), floor(y), ceil(z));
    glm::vec3 p3 = glm::vec3(floor(x), ceil(y),  floor(z));
    glm::vec3 p4 = glm::vec3(floor(x), ceil(y),  ceil(z));
    glm::vec3 p5 = glm::vec3(ceil(x),  floor(y), floor(z));
    glm::vec3 p6 = glm::vec3(ceil(x),  floor(y), ceil(z));
    glm::vec3 p7 = glm::vec3(ceil(x),  ceil(y),  floor(z));
    glm::vec3 p8 = glm::vec3(ceil(x),  ceil(y),  ceil(z));

    float v1 = smoothNoise(p1);
    float v2 = smoothNoise(p2);
    float v3 = smoothNoise(p3);
    float v4 = smoothNoise(p4);
    float v5 = smoothNoise(p5);
    float v6 = smoothNoise(p6);
    float v7 = smoothNoise(p7);
    float v8 = smoothNoise(p8);

    float i1 = cosInterp(v1 , v2 , z-floor(z));
    float i2 = cosInterp(v3 , v4 , z-floor(z));
    float i3 = cosInterp(v5 , v6 , z-floor(z));
    float i4 = cosInterp(v7 , v8 , z-floor(z));

    float i5 = cosInterp(i1 , i2 , y-floor(y));
    float i6 = cosInterp(i3 , i4 , y-floor(y));

    float interpNoise = cosInterp(i5 , i6 , x-floor(x));

    return interpNoise;
}

float noise3D(glm::vec3 p)
{
    float total = 0.0;
    //float persistence = 0.8;

    //Loop over n =4 octaves
    float i=0.0;
    for(int j=0; j< 20; j++)
    {
      if(j < int(5))
      {
        float frequency = pow(2.0, i);
        float amplitude = pow(.8f, i);
        i= i+1.0;
        //sum up all the octaves
        total += cosNoise(p * frequency) * (1.0/amplitude);
      }
    }
    return total;
}
