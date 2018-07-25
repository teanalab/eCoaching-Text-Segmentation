
#library
library(ggplot2)
library(reshape2)
library(pROC)

par(mfrow=c(1,2))
wd = '/home/mehedi/teana/projects/eCoaching-Text-Segmentation/plots/'

# melt data
# print AUC values
x1 <- read.csv(paste(wd,"roc/base/pred_brnn.csv", sep=''))
x2 <- read.csv(paste(wd,"roc/base/pred_mlp.csv", sep=''))
x3 <- read.csv(paste(wd,"roc/base/pred_crf.csv", sep=''))
x4 <- read.csv(paste(wd,"roc/base/pred_crnn.csv", sep=''))

roc_obj_brnn <- roc(x1$actual, x1$predict_prob)
roc_obj_mlp <- roc(x2$actual, x2$predict_prob)
roc_obj_crf <- roc(x3$actual, x3$predict_prob)
roc_obj_crnn <- roc(x4$actual, x4$predict_prob)

auc(roc_obj_brnn)
auc(roc_obj_mlp)
auc(roc_obj_crf)
auc(roc_obj_crnn)

# plot graph
plot(1-roc_obj_brnn$specificities, roc_obj_brnn$sensitivities, col='red', 
     type="l", ylab = "Sensitivity", xlab = "1-Specificity", cex.axis=1, cex.lab=1, lwd=1)
lines(1-roc_obj_mlp$specificities, roc_obj_mlp$sensitivities, col='blue', lty=1, lwd=1)
lines(1-roc_obj_crf$specificities, roc_obj_crf$sensitivities, col='green', lty=1, lwd=1)
lines(1-roc_obj_crnn$specificities, roc_obj_crnn$sensitivities, col='magenta', lty=1, lwd=1)
lines(0:1, 0:1, col='black', lty=5, lwd=1)
legend(0.77,0.30, c("BRNN", "MLP", "CRF", "CRNN"), 
       lty=c(1,1,1,1), col=c('red', 'blue', 'green', 'magenta'), cex=1, lwd=1)


# print AUC values
x1 <- read.csv(paste(wd,"roc/propose/pred_brnn.csv", sep=''))
x2 <- read.csv(paste(wd,"roc/propose/pred_mlp.csv", sep=''))
x3 <- read.csv(paste(wd,"roc/propose/pred_crf.csv", sep=''))
x4 <- read.csv(paste(wd,"roc/propose/pred_crnn.csv", sep=''))

roc_obj_brnn <- roc(x1$actual, x1$predict_prob)
roc_obj_mlp <- roc(x2$actual, x2$predict_prob)
roc_obj_crf <- roc(x3$actual, x3$predict_prob)
roc_obj_crnn <- roc(x4$actual, x4$predict_prob)

auc(roc_obj_brnn)
auc(roc_obj_mlp)
auc(roc_obj_crf)
auc(roc_obj_crnn)

# plot graph
plot(1-roc_obj_brnn$specificities, roc_obj_brnn$sensitivities, col='red', 
     type="l", ylab = "Sensitivity", xlab = "1-Specificity", cex.axis=1, cex.lab=1, lwd=1)
lines(1-roc_obj_mlp$specificities, roc_obj_mlp$sensitivities, col='blue', lty=1, lwd=1)
lines(1-roc_obj_crf$specificities, roc_obj_crf$sensitivities, col='green', lty=1, lwd=1)
lines(1-roc_obj_crnn$specificities, roc_obj_crnn$sensitivities, col='magenta', lty=1, lwd=1)
lines(0:1, 0:1, col='black', lty=5, lwd=1)
legend(0.77,0.30, c("BRNN", "MLP", "CRF", "CRNN"), 
       lty=c(1,1,1,1), col=c('red', 'blue', 'green', 'magenta'), cex=1, lwd=1)

print('done')
