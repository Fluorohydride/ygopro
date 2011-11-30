--ドラゴンに乗るワイバーン
function c3366982.initial_effect(c)
	--fusion material
	c:EnableReviveLimit()
	aux.AddFusionProcCode2(c,88819587,64428736,true,true)
	--direct attack
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_SINGLE)
	e2:SetCode(EFFECT_DIRECT_ATTACK)
	e2:SetCondition(c3366982.dircon)
	c:RegisterEffect(e2)
end
c3366982.material_count=2
c3366982.material={88819587,64428736}
function c3366982.dircon(e)
	return not Duel.IsExistingMatchingCard(c3366982.filter,e:GetHandler():GetControler(),0,LOCATION_MZONE,1,nil)
end
