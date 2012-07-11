--異次元竜 トワイライトゾーンドラゴン
function c50939127.initial_effect(c)
	--indestructable
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCode(EFFECT_INDESTRUCTABLE_EFFECT)
	e1:SetValue(c50939127.ind1)
	c:RegisterEffect(e1)
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_SINGLE)
	e2:SetCode(EFFECT_INDESTRUCTABLE_BATTLE)
	e2:SetValue(c50939127.ind2)
	c:RegisterEffect(e2)
end
function c50939127.ind1(e,re,rp,c)
	return not re:IsHasProperty(EFFECT_FLAG_CARD_TARGET) and re:IsActiveType(TYPE_SPELL+TYPE_TRAP)
end
function c50939127.ind2(e,c)
	return c:IsAttackBelow(1900)
end
