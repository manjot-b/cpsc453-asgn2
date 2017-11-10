#ifndef TEXTURE_H
#define TEXTURE_H
/*
*   A basic texture loader. You will have to 
*   manually bind the texture before using
* 
*/
class Texture
{
    public:
        Texture(const char *filename);
        unsigned int getID() const;
        int getWidth() const;
        int getHeight() const;

    private:
        unsigned int ID;
        int width, height;
};

#endif