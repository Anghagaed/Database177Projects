SELECT n_regionkey, r_regionkey
FROM nation, region
WHERE n_regionkey = r_regionkey