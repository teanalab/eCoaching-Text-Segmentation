#!/bin/bash

embpath="/media/mehedi/FEJ/Embeddings-deepecoach/ptbr"
fixedparams="--gpu -w 7 -e 20 -k 1 -b 1 -t bucket"
et="word2vec"
ed="600"
ea="sg"
ef="${embpath}/${et}/pt_${et}_${ea}_${ed}.emb"


# -----------
# SS
# ----------- 
# task options: ss/dd_fillers/dd_editdisfs
task="ss"
myid="SS_TEXT_CINDERELA"
time sudo python3 -m deepecoach --id $myid -d controle --models rcnn none --split-ratio 1 --task $task --save --emb-type $et --emb-file $ef $fixedparams
