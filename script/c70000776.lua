--ネクロバレーの祭殿
function c70000776.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCondition(c70000776.actcon)
	c:RegisterEffect(e1)
	--disable spsummon
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD)
	e2:SetCode(EFFECT_CANNOT_SPECIAL_SUMMON)
	e2:SetRange(LOCATION_SZONE)
	e2:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e2:SetTargetRange(1,1)
	e2:SetTarget(c70000776.sumlimit)
	c:RegisterEffect(e2)
	--self destroy
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_SINGLE)
	e3:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e3:SetRange(LOCATION_SZONE)
	e3:SetCode(EFFECT_SELF_DESTROY)
	e3:SetCondition(c70000776.sdcon)
	c:RegisterEffect(e3)
end
function c70000776.cfilter1(c)
	return c:IsFaceup() and c:IsSetCard(0x2e)
end
function c70000776.cfilter2(c)
	return c:IsFaceup() and c:IsCode(47355498)
end
function c70000776.actcon(e,tp,eg,ep,ev,re,r,rp)
	return Duel.IsExistingMatchingCard(c70000776.cfilter1,tp,LOCATION_MZONE,LOCATION_MZONE,1,nil)
		and (Duel.IsExistingMatchingCard(c70000776.cfilter2,tp,LOCATION_ONFIELD,LOCATION_ONFIELD,1,nil)
		or Duel.IsEnvironment(47355498))
end
function c70000776.sumlimit(e,c,sump,sumtype,sumpos,targetp)
	return not c:IsSetCard(0x2e)
end
function c70000776.sdcon(e)
	return not Duel.IsExistingMatchingCard(c70000776.cfilter1,0,LOCATION_MZONE,LOCATION_MZONE,1,nil)
		or (not Duel.IsExistingMatchingCard(c70000776.cfilter2,0,LOCATION_ONFIELD,LOCATION_ONFIELD,1,nil)
		and not Duel.IsEnvironment(47355498))
end
