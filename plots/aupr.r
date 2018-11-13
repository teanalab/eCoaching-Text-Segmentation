
#library
library(ggplot2)
library(reshape2)
library(PRROC)

par(mfrow=c(1,2))
wd = '/home/mehedi/teana/projects/eCoaching-Text-Segmentation/plots/'

# melt data
x1 <- read.csv(paste(wd,"roc/base/pred_brnn.csv", sep=''))
x2 <- read.csv(paste(wd,"roc/base/pred_mlp.csv", sep=''))
x3 <- read.csv(paste(wd,"roc/base/pred_crf.csv", sep=''))
x4 <- read.csv(paste(wd,"roc/base/pred_crnn.csv", sep=''))

curve_obj_brnn <- pr.curve(scores.class0 = x1[x1$actual == 1,]$predict_prob, scores.class1 = x1[x1$actual == 0,]$predict_prob, curve = T)
curve.points.brnn<-curve_obj_brnn$curve

curve_obj_mlp <- pr.curve(scores.class0 = x2[x2$actual == 1,]$predict_prob, scores.class1 = x2[x2$actual == 0,]$predict_prob, curve = T)
curve.points.mlp<-curve_obj_mlp$curve

curve_obj_crf <- pr.curve(scores.class0 = x3[x3$actual == 1,]$predict_prob, scores.class1 = x3[x3$actual == 0,]$predict_prob, curve = T)
curve.points.crf<-curve_obj_crf$curve

curve_obj_crnn <- pr.curve(scores.class0 = x4[x4$actual == 1,]$predict_prob, scores.class1 = x4[x4$actual == 0,]$predict_prob, curve = T)
curve.points.crnn<-curve_obj_crnn$curve

# print AUPR values
print(curve_obj_brnn)
print(curve_obj_mlp)
print(curve_obj_crf)
print(curve_obj_crnn)

# plot graph
plot(curve.points.brnn[,1], curve.points.brnn[,2], col='red', xlim=c(0, 1), ylim=c(0, 1),  
     type="l", ylab = "Precision", xlab = "Recall", cex.axis=1, cex.lab=1, lwd=1)
lines(curve.points.mlp[,1], curve.points.mlp[,2], col='blue', lty=1, lwd=1)
lines(curve.points.crf[,1], curve.points.crf[,2], col='green', lty=1, lwd=1)
lines(curve.points.crnn[,1], curve.points.crnn[,2], col='magenta', lty=1, lwd=1)
legend(0.0,0.36, c("BRNN", "MLP", "CRF", "CRNN"), 
       lty=c(1,1,1,1), col=c('red', 'blue', 'green', 'magenta'), cex=1, lwd=1)


xx1 <- read.csv(paste(wd,"roc/propose/pred_brnn.csv", sep=''))
xx2 <- read.csv(paste(wd,"roc/propose/pred_mlp.csv", sep=''))
xx3 <- read.csv(paste(wd,"roc/propose/pred_crf.csv", sep=''))
xx4 <- read.csv(paste(wd,"roc/propose/pred_crnn.csv", sep=''))


curve_obj_brnn <- pr.curve(scores.class0 = xx1[xx1$actual == 1,]$predict_prob, scores.class1 = xx1[xx1$actual == 0,]$predict_prob, curve = T)
curve.points.brnn<-curve_obj_brnn$curve

curve_obj_mlp <- pr.curve(scores.class0 = xx2[xx2$actual == 1,]$predict_prob, scores.class1 = xx2[xx2$actual == 0,]$predict_prob, curve = T)
curve.points.mlp<-curve_obj_mlp$curve

curve_obj_crf <- pr.curve(scores.class0 = xx3[xx3$actual == 1,]$predict_prob, scores.class1 = xx3[xx3$actual == 0,]$predict_prob, curve = T)
curve.points.crf<-curve_obj_crf$curve

curve_obj_crnn <- pr.curve(scores.class0 = xx4[xx4$actual == 1,]$predict_prob, scores.class1 = xx4[xx4$actual == 0,]$predict_prob, curve = T)
curve.points.crnn<-curve_obj_crnn$curve

# print AUPR values
print(curve_obj_brnn)
print(curve_obj_mlp)
print(curve_obj_crf)
print(curve_obj_crnn)

# plot graph
plot(curve.points.brnn[,1], curve.points.brnn[,2], col='red', xlim=c(0, 1), ylim=c(0, 1),  
     type="l", ylab = "Precision", xlab = "Recall", cex.axis=1, cex.lab=1, lwd=1)
lines(curve.points.mlp[,1], curve.points.mlp[,2], col='blue', lty=1, lwd=1)
lines(curve.points.crf[,1], curve.points.crf[,2], col='green', lty=1, lwd=1)
lines(curve.points.crnn[,1], curve.points.crnn[,2], col='magenta', lty=1, lwd=1)
legend(0.0,0.36, c("BRNN", "MLP", "CRF", "CRNN"), 
       lty=c(1,1,1,1), col=c('red', 'blue', 'green', 'magenta'), cex=1, lwd=1)

print('done')

