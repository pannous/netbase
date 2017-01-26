#mv amazon amazon.old
#mkdir amazon
# rsync --ignore-existing root@909.hqgmbh.eu:/datasearch1/amazon-direct/* . #amazon 15G	total
# rsync --ignore-existing root@909.hqgmbh.eu:/datasearch1/amazon-direct/*.base.csv.gz amazon/ #1.6G  total

# softlinks
cd amazon
for file in `ls *2016*.base.csv.gz`; do
  echo $file
  neu=${file/_20[0-9][0-9][0-9][0-9][0-9][0-9]/} # remove date
  echo $neu
  ln -f -s $file $neu
done;
