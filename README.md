# eCoaching Text Segmentation

We propose a new method for the segmentation of text collected from eCoaching.

## Prerequisite
  
  * lemur

## Compile Project

  * Open terminal from the src folder of the project
  * Change the project directory from Makefile
  * Run "make clean"
  * Run "make all"

## Running Models and Evaluate Results

Now, you have all the executables in your project bin directory. Go to the project bin directory first.
  
  * Open all configuration files and check the directories mentined inside
  * Format input data for LDA model
```
 ./Formatter ../config/stem/format.cfg
```
  * Split the dataset into train and test sets. You can modify the cfg file for KFolds, deafult value sets as 5. Run 
```
./Splitter  ../config/stem/split.cfg
```
  * Modify all configuration files from config/stem for Each model, you can modify the alpha and beta value from this configuration file. You need 10 cfg files for each model if 10 folds is used. Run one of the model from LabLDA, MGLabLDA and LTA :
```
./LabLDA ../config/stem/lda-1.cfg
```
You can create a bash file and run 10 folds one by one with a single line command. After this run you will get word distribution. 
  * You need 10 class cfg files for each model if 10 folds is used. Run one of the model from LDAClass, MGLDAClass and LTAClass :
```
./LDAClass ../config/stem/lda-cls-1.cfg
```
You can create a bash file and run 10 folds one by one with a single line command. After this run you will get classification results.   
  * Modify configuration file from config/stem for evaluation. Run
```
./Eval ../config/stem/lda-eval.cfg
```
  * Performance of the model will be  saved into "perf" folder


N.B. : If you are using stopwords or stem and stop together, then you will have to modify the corresponding file such as "../config/stop-stem/split.cfg"

## Databases and Results
 * data : contains the raw/stem/stop-stem data
 * splits: contains train and test data after k folds splitting
 * stats: statistics of train and test datasets 
 * topic: topic distributions
 * class: classification results of test data
 * perf: performance detail of the models

## Authors

* **[Mehedi Hasan](http://www.cs.wayne.edu/mehedi/)** - *Department of Computer Science at Wayne State University*
* **[Alexander Kotov](http://www.cs.wayne.edu/kotov/)** - *Department of Computer Science at Wayne State University*


