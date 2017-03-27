SELECT SUM(c_acctbal), c_custkey FROM customer WHERE c_acctbal > 0.0 GROUP BY c_custkey

