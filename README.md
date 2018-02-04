# Adolescent Obesity AMIA 2015

We propose Latent Class Allocation (LCA) and Discriminative Labeled Latent Dirichlet Allocation (DL-LDA), two novel interpretable probabilistic latent variable models for automatic annotation of clinical text. Both models separate the terms that are highly characteristic of textual fragments annotated with a given set of labels from other non-discriminative terms, but rely on generative processes with different structure of latent variables. LCA directly learns class-specific multinomials, while DL-LDA breaks them down into topics (clusters of semantically related words). Extensive experimental evaluation indicates that the proposed models outperform Naïve Bayes, a standard probabilistic classifier, and Labeled LDA, a state-of-the-art topic model for labeled corpora, on the task of automatic annotation of transcripts of motivational interviews, while the output of the proposed models can be easily interpreted by clinical practitioners.


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
  * Split the dataset into train and test sets. You can modify the cfg file for KFolds, deafult value sets as 5. Run \ 
		"./Splitter  &lt;../config/stem/split.cfg&gt;"
  * Modify all configuration files from config/stem for Each model, you can modify the alpha and beta value from this configuration file. You need 10 cfg files for each model if 10 folds is used. Run one of the model from LabLDA, MGLabLDA and LTA : \
		"./LabLDA &lt;../config/stem/lda-1.cfg&gt;"; 
	You can create a bash file and run 10 folds one by one with a single line command. After this run you will get word distribution. 
  * You need 10 class cfg files for each model if 10 folds is used. Run one of the model from LDAClass, MGLDAClass and LTAClass : \ 
		"./LDAClass &lt;../config/stem/lda-cls-1.cfg&gt;"; 
	You can create a bash file and run 10 folds one by one with a single line command. After this run you will get classification results.   
  * Modify configuration file from config/stem for evaluation. Run \
		"./Eval  &lt;../config/stem/lda-eval.cfg&gt;"
  * Performance of the model will be  saved into "perf" folder


	N.B. : If you are using stopwords or stem and stop together, then you will have to modify the corresponding file such as "../config/stop-stem/split.cfg"

## Citation

Please cite the following paper if you use our code or ideas in your work:

Alexander Kotov, Mehedi Hasan, April Carcone, Ming Dong, Sylvie Naar-King and Kathryn Brogan Hartlieb, "Interpretable Probabilistic Latent Variable Models for Automatic Annotation of Clinical Text", full paper, In Proceedings of the 2015 Annual Symposium of the American Medical Informatics Association (AMIA'15), pages 785-794.
