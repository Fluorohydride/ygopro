--DDD神託王ダルク
function c82956492.initial_effect(c)
	--fustion material
	aux.AddFusionProcFunRep(c,aux.FilterBoolFunction(Card.IsSetCard,0xaf),2,true)
	c:EnableReviveLimit()
	--damage conversion
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD)
	e2:SetCode(EFFECT_REVERSE_DAMAGE)
	e2:SetRange(LOCATION_MZONE)
	e2:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e2:SetTargetRange(1,0)
	e2:SetValue(c82956492.rev)
	c:RegisterEffect(e2)
end
function c82956492.rev(e,re,r,rp,rc)
	return bit.band(r,REASON_EFFECT)~=0
end
