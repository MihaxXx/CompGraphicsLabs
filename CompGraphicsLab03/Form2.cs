﻿using System;
using System.Drawing;
using System.Windows.Forms;

namespace CompGraphicsLab03
{
    public partial class Form2 : Form
    {
        private Form1 _form1;
        private Graphics g;
        Pen pen, penFill;
        public Form2(Form1 form1)
        {
            _form1 = form1;
            InitializeComponent();
            pen = new Pen(Color.Black, 1);
            penFill = new Pen(Color.Red, 1);
            pictureBox1.Image = new Bitmap(pictureBox1.Width, pictureBox1.Height);
            g = Graphics.FromImage(pictureBox1.Image);
        }

        private void button1_Click(object sender, EventArgs e)
        {
            this.Visible = false;
            _form1.Visible = true;
        }

        private void Form2_FormClosing(object sender, FormClosingEventArgs e)
        {
            Application.Exit();
        }
        private void button2_Click(object sender, EventArgs e)
        {
            if (colorDialog1.ShowDialog() == DialogResult.OK)
            {
                penFill.Color = colorDialog1.Color;
                button2.BackColor = colorDialog1.Color;
            }
        }
        private Point old;
        private bool drawing = false;

        private Color areaToFillColor;
        private void pictureBox1_MouseClick(object sender, MouseEventArgs e)
        {
            areaToFillColor = ((Bitmap)pictureBox1.Image).GetPixel(e.X, e.Y);

            if (checkBox1.Checked)
                fillAreaPic(e.X, e.Y);
            else
            {
                // Исключаем повторую заливку тем же цветом
                if (!(areaToFillColor.R == penFill.Color.R && areaToFillColor.G == penFill.Color.G
                    && areaToFillColor.B == penFill.Color.B))
                    fillArea(e.X, e.Y);
            }
        }

        enum Direction
        {
            left, right
        };
        /// <summary>
        /// Копирует линию из рисунка для заливки на заливаемую область
        /// </summary>
        /// <param name="x">Координата х заливаемой области</param>
        /// <param name="y">Координата y заливаемой области</param>
        /// <param name="px">Координата х рисунка для заливки</param>
        /// <param name="py">Координата y рисунка для заливки</param>
        /// <param name="d">Направление движения: влево или вправо</param>
        /// <returns>Новые позиции х-координат для считывания</returns>
        (int x, int px) CopyLine(int x, int y, int px, int py, Direction d)
        {
            while (x > 0 && x < pictureBox1.Image.Width)
            {
                if (((Bitmap)pictureBox1.Image).GetPixel(x, y) != areaToFillColor
                    || ((Bitmap)pictureBox1.Image).GetPixel(x, y) == pen.Color)
                    break;

                if (px < 0)
                    px += bmpFill.Width;
                else if (px >= bmpFill.Width)
                    px %= bmpFill.Width;


                Color c =  bmpFill.GetPixel(px, py);
                ((Bitmap)pictureBox1.Image).SetPixel(x, y, c);
                if (d == Direction.right)
                {
                    x++;
                    px++;
                }
                else
                {
                    x--;
                    px--;
                }
            }
            pictureBox1.Invalidate();
            return (x, px);
        }
        void FillPicHelp(int x, int y, int px, int py)
        {

            Bitmap bitmap = pictureBox1.Image as Bitmap;
            if (bitmap.GetPixel(x, y) != areaToFillColor || bitmap.GetPixel(x, y) == pen.Color)
                return;

            if (py < 0)
                py += bmpFill.Height;
            else if (py >= bmpFill.Height)
                py %= bmpFill.Height;


            (int x_left, int px_left) = CopyLine(x, y, px, py, Direction.left);
            (int x_right, _) = CopyLine(x + 1, y, px + 1, py, Direction.right);

            if (y + 1 < bitmap.Height)
                for (int i = x_left + 1, j = px_left + 1; i < x_right; ++i, ++j)
                    FillPicHelp(i, y + 1, j, py + 1);

            if (y - 1 > 0)
                for (int i = x_left + 1, j = px_left + 1; i < x_right; ++i, ++j)
                    FillPicHelp(i, y - 1, j, py - 1);

            pictureBox1.Invalidate();
        }
        Bitmap bmpFill;
        void fillAreaPic(int x, int y)
        {
            FillPicHelp(x, y, bmpFill.Width / 2, bmpFill.Height / 2);
        }

        void fillArea(int x, int y)
        {
            Bitmap bitmap = pictureBox1.Image as Bitmap;

            Color currentColor = bitmap.GetPixel(x, y);
            if (currentColor != areaToFillColor || currentColor == penFill.Color)
                return;

            int x_left = x;
            while (x_left > 0 && bitmap.GetPixel(x_left, y) == areaToFillColor)
            {
                x_left--;
            }

            int x_right = x;
            while (x_right < bitmap.Size.Width && bitmap.GetPixel(x_right, y) == areaToFillColor)
            {
                x_right++;
            }
            g.DrawLine(penFill, x_left + 1, y, x_right - 1, y);

            if (y + 1 < bitmap.Height)
                for (int i = x_left + 1; i < x_right; ++i)
                    fillArea(i, y + 1);

            if (y - 1 > 0)
                for (int i = x_left + 1; i < x_right; ++i)
                    fillArea(i, y - 1);

            pictureBox1.Invalidate();
        }


        private void pictureBox1_MouseMove(object sender, MouseEventArgs e)
        {
            if (drawing)
            {
                g.DrawLine(pen, old.X, old.Y, e.X, e.Y);
                old = e.Location;
                pictureBox1.Invalidate();
            }
        }

        private void pictureBox1_MouseDown(object sender, MouseEventArgs e)
        {
            old = e.Location;
            drawing = true;
        }

        private void pictureBox1_MouseUp(object sender, MouseEventArgs e)
        {
            drawing = false;
        }

        private void button3_Click(object sender, EventArgs e)
        {
            g.Clear(Color.White);
            pictureBox1.Invalidate();
        }

        private void button4_Click(object sender, EventArgs e)
        {

            OpenFileDialog ofd = new OpenFileDialog
            {
                // Маска для файлов
                Filter = "Image Files(*.BMP;*.JPG;*.GIF;*.PNG)|*.BMP;*.JPG;*.GIF;*.PNG|All files (*.*)|*.*"
            };
            if (ofd.ShowDialog() == DialogResult.OK)
            {
                try
                {
                    Image imgFill = Image.FromFile(ofd.FileName);
                    bmpFill = new Bitmap(imgFill);//, pictureBox1.Width, pictureBox1.Height);
                }
                catch
                {
                    MessageBox.Show("Невозможно открыть выбранный файл", "Ошибка",
                        MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
            }

        }

        private void checkBox1_CheckedChanged(object sender, EventArgs e)
        {
            button4.Enabled = (sender as CheckBox).Checked;
            button2.Enabled = !(sender as CheckBox).Checked;
            button2.BackColor = DefaultBackColor;
        }
    }
}
