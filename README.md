# QR-PCA-FaceRec

A face recognition algorithm using QR based PCA.

## Requirements

### Visual Studio
The solution is created with Visual Studio 2012. You are more than welcome to use other IDEs. All you need to do is to copy all the codes from QR-PCA-FaceRec.cpp and link the dependencies correctly.

### [OpenCV](http://opencv.org/downloads.html)
I'm using `OpenCV 3.1`, while other version of OpenCV may work as well, since I only use its basic I/O and Mat functionalities.

### [Armadillo](http://arma.sourceforge.net/)
A high quality linear algebra library (matrix maths) for the C++ language.

Please note that `Armadillo` requires `LAPACK` for matrix decompositions and [boost](http://www.boost.org/). I'm using [Intel MKL](https://software.intel.com/en-us/intel-mkl) through Student Program. You may choose any LAPACK replacement you want.

## How to use

Take these 5 steps:

* Download the "AT&T Database of Faces" from [here](http://www.cl.cam.ac.uk/research/dtg/attarchive/facedatabase.html) and place it anywhere you want.

* Change the `orl_path` at line  to your AT&T Database folder.

* Set number of dimensions you're aim for by changing the `component_num` at line  .

* Choose which measure of distance you want to use at line  and .

* Build and run!
 
## License

HanyNet is released under the MIT license. Considering the dependencies are both open-source, you are granted with the same rights as they offer.
