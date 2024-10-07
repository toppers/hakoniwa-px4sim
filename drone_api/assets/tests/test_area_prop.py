import sys
import os
# libディレクトリをモジュール検索パスに追加
sys.path.append(os.path.join(os.path.dirname(__file__), '../lib'))

import unittest
from lib.hako_area_pro_accessor_impl import HakoAreaPropAccessorImpl
from lib.hako_area_property import HakoAreaProperty

class TestHakoAreaPropAccessor(unittest.TestCase):

    def setUp(self):
        # テスト用のJSONファイルのパスを指定
        self.json_file_path = 'tests/test_data/area_property.json'
        self.prop_accessor = HakoAreaPropAccessorImpl(self.json_file_path)

    def test_load_property_data(self):
        """プロパティデータが正しく読み込まれるかを確認"""
        # area_id "area_1" のプロパティを確認
        area_1_property = self.prop_accessor.get_property("area_1")
        self.assertIsNotNone(area_1_property, "area_1 のプロパティが存在しません。")
        self.assertEqual(area_1_property.get_wind_velocity(), (5.0, 0.0, 0.0))
        self.assertEqual(area_1_property.get_temperature(), 22.5)

    def test_missing_property(self):
        """プロパティが存在しない場合の処理を確認"""
        # area_id "area_4" の風速や温度が存在しない場合
        area_4_property = self.prop_accessor.get_property("area_4")
        self.assertIsNotNone(area_4_property, "area_4 のプロパティが読み込まれませんでした。")
        self.assertIsNone(area_4_property.get_wind_velocity(), "風速がNoneであるべき")
        self.assertIsNone(area_4_property.get_temperature(), "温度がNoneであるべき")

    def test_non_existent_area(self):
        """存在しないエリアIDを指定した場合の処理を確認"""
        # 存在しないエリアIDを指定した場合、Noneを返す
        area_property = self.prop_accessor.get_property("area_non_existent")
        self.assertIsNone(area_property, "存在しないエリアIDの場合、Noneが返されるべき")

if __name__ == '__main__':
    unittest.main()
