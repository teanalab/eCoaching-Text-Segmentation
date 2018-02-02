# Adolescent Obesity AMIA 2015

We propose Latent Class Allocation (LCA) and Discriminative Labeled Latent Dirichlet Allocation (DL-LDA), two novel interpretable probabilistic latent variable models for automatic annotation of clinical text. Both models separate the terms that are highly characteristic of textual fragments annotated with a given set of labels from other non-discriminative terms, but rely on generative processes with different structure of latent variables. LCA directly learns class-specific multinomials, while DL-LDA breaks them down into topics (clusters of semantically related words). Extensive experimental evaluation indicates that the proposed models outperform Naïve Bayes, a standard probabilistic classifier, and Labeled LDA, a state-of-the-art topic model for labeled corpora, on the task of automatic annotation of transcripts of motivational interviews, while the output of the proposed models can be easily interpreted by clinical practitioners.

## Compile Project

  * Open terminal in the src folder of the project
  * Run "make clean"
  * Run "make all"

## Running Models and Evaluate Results

Now, you have all the executables in your project bin directory. Go to the project bin directory first.
  
  * Check all configuration files and their directories
  * Run one of the model from LabLDA, MGLabLDA and LTA : "./LabLDA &lt;cfg file &gt;"
  * Run "./Eval  &lt;cfg file &gt;"
  * Performance of the model will be  saved into "perf" folder


## Citation

Please cite the following paper if you use our code or ideas in your work:

Alexander Kotov, Mehedi Hasan, April Carcone, Ming Dong, Sylvie Naar-King and Kathryn Brogan Hartlieb, "Interpretable Probabilistic Latent Variable Models for Automatic Annotation of Clinical Text", full paper, In Proceedings of the 2015 Annual Symposium of the American Medical Informatics Association (AMIA'15), pages 785-794.
