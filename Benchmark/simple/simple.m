axis equal;
hold on;
grid on;

block_x=[ 0 8 8 0 0];
block_y=[ 0 0 8 8 0];
fill(block_x,block_y,'c');
text(4,4, 'o0', 'HorizontalAlignment', 'center', 'VerticalAlignment', 'middle');

block_x=[ 8 14 14 8 8];
block_y=[ 0 0 4 4 0];
fill(block_x,block_y,'c');
text(11,2, 'o1', 'HorizontalAlignment', 'center', 'VerticalAlignment', 'middle');

block_x=[ 14 18 18 14 14];
block_y=[ 0 0 10 10 0];
fill(block_x,block_y,'c');
text(16,5, 'o2', 'HorizontalAlignment', 'center', 'VerticalAlignment', 'middle');

block_x=[ 0 10 10 0 0];
block_y=[ 8 8 14 14 8];
fill(block_x,block_y,'c');
text(5,11, 'o3', 'HorizontalAlignment', 'center', 'VerticalAlignment', 'middle');

block_x=[ 10 16 16 10 10];
block_y=[ 10 10 16 16 10];
fill(block_x,block_y,'c');
text(13,13, 'o4', 'HorizontalAlignment', 'center', 'VerticalAlignment', 'middle');

block_x=[ 0 6 6 0 0];
block_y=[ 14 14 20 20 14];
fill(block_x,block_y,'c');
text(3,17, 'o5', 'HorizontalAlignment', 'center', 'VerticalAlignment', 'middle');

