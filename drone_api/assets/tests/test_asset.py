import sys
import os
# libディレクトリをモジュール検索パスに追加
sys.path.append(os.path.join(os.path.dirname(__file__), '../lib'))

import unittest
from lib.hako_area_accessor_impl import HakoAreaAccessorImpl
from lib.hako_area_pro_accessor_impl import HakoAreaPropAccessorImpl
from lib.hako_aabb_object_space import HakoAABBObjectSpace

class TestAssetIntegration(unittest.TestCase):

    def setUp(self):
        # area.jsonとarea_property.jsonを読み込んで、Implクラスをインスタンス化
        self.area_accessor = HakoAreaAccessorImpl('tests/test_data/area.json')
        self.prop_accessor = HakoAreaPropAccessorImpl('tests/test_data/area_property.json')

    def test_area_matching_with_property(self):
        """正常系：エリアに一致し、対応するプロパティが正しく取得されるかをテスト"""
        # ドローンの位置とサイズをインスタンス化
        drone_position = (50, 50, 25)  # area_1の範囲内
        drone_size = (10, 10, 10)
        object_space = HakoAABBObjectSpace(drone_position, drone_size)

        # エリアマッチング
        matched_area_id = self.area_accessor.get_area_id(object_space)
        self.assertEqual(matched_area_id, "area_1", "エリアマッチングが正しく行われませんでした。")

        # プロパティ情報の取得
        property_info = self.prop_accessor.get_property(matched_area_id)
        self.assertIsNotNone(property_info, "プロパティ情報が取得できませんでした。")
        self.assertEqual(property_info.get_wind_velocity(), (5.0, 0.0, 0.0))
        self.assertEqual(property_info.get_temperature(), 22.5)

    def test_no_area_match(self):
        """異常系：どのエリアにも一致しない場合、Noneが返るかをテスト"""
        # ドローンの位置とサイズをエリア外に設定
        drone_position = (250, 250, 25)  # どのエリアにも属さない
        drone_size = (10, 10, 10)
        object_space = HakoAABBObjectSpace(drone_position, drone_size)

        # エリアマッチング
        matched_area_id = self.area_accessor.get_area_id(object_space)
        self.assertIsNone(matched_area_id, "どのエリアにも属さない場合、Noneが返されるべきです。")

    def test_area_matching_with_missing_property(self):
        """異常系：エリアに一致するが、プロパティ情報が存在しない場合をテスト"""
        # ドローンの位置とサイズをarea_4に設定（プロパティ情報が存在しないエリア）
        drone_position = (150, 150, 25)  # area_4の範囲内
        drone_size = (10, 10, 10)
        object_space = HakoAABBObjectSpace(drone_position, drone_size)

        # エリアマッチング
        matched_area_id = self.area_accessor.get_area_id(object_space)
        self.assertEqual(matched_area_id, "area_4", "エリアマッチングが正しく行われませんでした。")

        # プロパティ情報の取得
        property_info = self.prop_accessor.get_property(matched_area_id)
        self.assertIsNotNone(property_info, "プロパティ情報がNoneであるべきではありません。")
        self.assertIsNone(property_info.get_wind_velocity(), "風速がNoneであるべきです。")
        self.assertIsNone(property_info.get_temperature(), "温度がNoneであるべきです。")

if __name__ == '__main__':
    unittest.main()
