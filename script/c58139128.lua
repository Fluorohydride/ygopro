--墓守の祈祷師
function c58139128.initial_effect(c)
	--def up
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_UPDATE_DEFENCE)
	e1:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e1:SetRange(LOCATION_MZONE)
	e1:SetValue(c58139128.defval)
	c:RegisterEffect(e1)
	--disable
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e2:SetRange(LOCATION_MZONE)
	e2:SetCode(EVENT_CHAIN_ACTIVATING)
	e2:SetOperation(c58139128.disop)
	c:RegisterEffect(e2)
	--cannot activate
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_FIELD)
	e3:SetCode(EFFECT_CANNOT_ACTIVATE)
	e3:SetRange(LOCATION_MZONE)
	e3:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e3:SetTargetRange(0,1)
	e3:SetCondition(c58139128.econ)
	e3:SetValue(c58139128.efilter1)
	c:RegisterEffect(e3)
	--
	local e4=Effect.CreateEffect(c)
	e4:SetType(EFFECT_TYPE_FIELD)
	e4:SetCode(EFFECT_INDESTRUCTABLE_EFFECT)
	e4:SetRange(LOCATION_MZONE)
	e4:SetProperty(EFFECT_FLAG_IGNORE_RANGE+EFFECT_FLAG_SET_AVAILABLE)
	e4:SetCondition(c58139128.econ)
	e4:SetTarget(c58139128.etarget)
	e4:SetValue(c58139128.efilter2)
	c:RegisterEffect(e4)
end
function c58139128.filter(c)
	return c:IsSetCard(0x2e) and c:IsType(TYPE_MONSTER)
end
function c58139128.defval(e,c)
	return Duel.GetMatchingGroupCount(c58139128.filter,c:GetControler(),LOCATION_GRAVE,0,nil)*200
end
function c58139128.disop(e,tp,eg,ep,ev,re,r,rp)
	local loc=Duel.GetChainInfo(ev,CHAININFO_TRIGGERING_LOCATION)
	if re:IsActiveType(TYPE_MONSTER) and not re:GetHandler():IsSetCard(0x2e) and loc==LOCATION_GRAVE then
		Duel.NegateEffect(ev)
	end
end
function c58139128.actfilter(c)
	return c:IsFaceup() and c:IsCode(47355498)
end
function c58139128.econ(e)
	return Duel.IsExistingMatchingCard(c58139128.actfilter,0,LOCATION_ONFIELD,LOCATION_ONFIELD,1,nil)
		or Duel.IsEnvironment(47355498)
end
function c58139128.efilter1(e,re,tp)
	return re:GetHandler():IsType(TYPE_FIELD) and re:IsHasType(EFFECT_TYPE_ACTIVATE)
end
function c58139128.etarget(e,c)
	return c:IsType(TYPE_FIELD)
end
function c58139128.efilter2(e,re,tp)
	return e:GetOwnerPlayer()~=re:GetOwnerPlayer()
end
