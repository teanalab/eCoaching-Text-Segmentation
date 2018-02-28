
#library
library(ggplot2)
library(reshape2)
library(pROC)

par(mfrow=c(1,2))

# melt data
r1 <- read.csv("/home/mehedi/teana/projects/eCoaching-Text-Segmentation/results/roc/False/KNN3False-ROC.csv")
r2 <- read.csv("/home/mehedi/teana/projects/eCoaching-Text-Segmentation/results/roc/False/SVMFalse-ROC.csv")
r3 <- read.csv("/home/mehedi/teana/projects/eCoaching-Text-Segmentation/results/roc/False/NBMFalse-ROC.csv")

# print AUC values
x1 <- read.csv("/home/mehedi/teana/projects/eCoaching-Text-Segmentation/results/roc/False/auc_RNN-LSTMFalse-ROC.csv")
x2 <- read.csv("/home/mehedi/teana/projects/eCoaching-Text-Segmentation/results/roc/False/auc_RNN-GRUFalse-ROC.csv")
roc_obj_lstm <- roc(x1$actual, x1$prediction)
auc(roc_obj_lstm)

roc_obj_gru <- roc(x2$actual, x2$prediction)
auc(roc_obj_gru)

# plot graph
plot(r1$fpr, r1$tpr, col='red', type="l", ylab = "Sensitivity", xlab = "1-Specificity", cex.axis=1, cex.lab=1, lwd=1)
lines(r2$fpr, r2$tpr, col='blue', lty=1, lwd=1)
lines(r3$fpr, r3$tpr, col='green', lty=1, lwd=1)
lines(1-roc_obj_lstm$specificities, roc_obj_lstm$sensitivities, col='pink', lty=1, lwd=1)
lines(1-roc_obj_gru$specificities, roc_obj_gru$sensitivities, col='purple', lty=1, lwd=1)
lines(0:1, 0:1, col='black', lty=5, lwd=1)
legend(0.73,0.53, c("KNN", "SVM", "NBM", "LSTM", "GRU"), 
       lty=c(1,1,1,1,1), col=c('red', 'blue', 'green', 'pink', 'purple'), cex=1, lwd=1)

# print AUC values
x1 <- read.csv("/home/mehedi/teana/projects/eCoaching-Text-Segmentation/results/roc/False/auc_RNN-LSTMFalse-ROC.csv")
x2 <- read.csv("/home/mehedi/teana/projects/eCoaching-Text-Segmentation/results/roc/False/auc_RNN-GRUFalse-ROC.csv")
roc_obj_lstm <- roc(x1$actual, x1$prediction)
auc(roc_obj_lstm)

roc_obj_gru <- roc(x2$actual, x2$prediction)
auc(roc_obj_gru)

# melt data
r1 <- read.csv("/home/mehedi/teana/projects/eCoaching-Text-Segmentation/results/roc/True/KNN3True-ROC.csv")
r2 <- read.csv("/home/mehedi/teana/projects/eCoaching-Text-Segmentation/results/roc/True/SVMTrue-ROC.csv")
r3 <- read.csv("/home/mehedi/teana/projects/eCoaching-Text-Segmentation/results/roc/True/NBMTrue-ROC.csv")

# plot graph
plot(r1$fpr, r1$tpr, col='red', type="l", ylab = "Sensitivity", xlab = "1-Specificity", cex.axis=1, cex.lab=1, lwd=1)
lines(r2$fpr, r2$tpr, col='blue', lty=1, lwd=1)
lines(r3$fpr, r3$tpr, col='green', lty=1, lwd=1)
lines(1-roc_obj_lstm$specificities, roc_obj_lstm$sensitivities, col='pink', lty=1, lwd=1)
lines(1-roc_obj_gru$specificities, roc_obj_gru$sensitivities, col='purple', lty=1, lwd=1)
lines(0:1, 0:1, col='black', lty=5, lwd=1)
legend(0.73,0.53, c("KNN", "SVM", "NBM", "LSTM", "GRU"), 
       lty=c(1,1,1,1,1), col=c('red', 'blue', 'green', 'pink', 'purple'), cex=1, lwd=1)

