--セフィラの聖選士
function c98076754.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetProperty(EFFECT_FLAG_DAMAGE_STEP)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(TIMING_DAMAGE_STEP)
	e1:SetCondition(c98076754.condition)
	c:RegisterEffect(e1)
	--Atk up
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD)
	e2:SetRange(LOCATION_SZONE)
	e2:SetTargetRange(LOCATION_MZONE,0)
	e2:SetCode(EFFECT_UPDATE_ATTACK)
	e2:SetCondition(c98076754.effcon)
	e2:SetValue(c98076754.atkval)
	e2:SetLabel(3)
	c:RegisterEffect(e2)
	--indes
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_FIELD)
	e3:SetCode(EFFECT_INDESTRUCTABLE_EFFECT)
	e3:SetProperty(EFFECT_FLAG_SET_AVAILABLE)
	e3:SetRange(LOCATION_SZONE)
	e3:SetTargetRange(LOCATION_MZONE,0)
	e3:SetCondition(c98076754.effcon)
	e3:SetValue(c98076754.indval)
	e3:SetLabel(5)
	c:RegisterEffect(e3)
	--cannot be target
	local e4=Effect.CreateEffect(c)
	e4:SetType(EFFECT_TYPE_FIELD)
	e4:SetCode(EFFECT_CANNOT_BE_EFFECT_TARGET)
	e4:SetProperty(EFFECT_FLAG_SET_AVAILABLE)
	e4:SetRange(LOCATION_SZONE)
	e4:SetTargetRange(LOCATION_MZONE,0)
	e4:SetCondition(c98076754.effcon)
	e4:SetValue(aux.tgoval)
	e4:SetLabel(8)
	c:RegisterEffect(e4)
	--todeck
	local e5=Effect.CreateEffect(c)
	e5:SetCategory(CATEGORY_TODECK)
	e5:SetType(EFFECT_TYPE_QUICK_O)
	e5:SetCode(EVENT_FREE_CHAIN)
	e5:SetRange(LOCATION_SZONE)
	e5:SetCondition(c98076754.tdcon)
	e5:SetCost(c98076754.tdcost)
	e5:SetTarget(c98076754.tdtg)
	e5:SetOperation(c98076754.tdop)
	c:RegisterEffect(e5)
end
function c98076754.condition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetCurrentPhase()~=PHASE_DAMAGE or not Duel.IsDamageCalculated()
end
function c98076754.cfilter(c)
	return c:IsFaceup() and c:IsSetCard(0xc4)
end
function c98076754.effcon(e)
	return Duel.GetMatchingGroup(c98076754.cfilter,e:GetHandlerPlayer(),LOCATION_EXTRA,0,nil):GetClassCount(Card.GetCode)>=e:GetLabel()
end
function c98076754.atkval(e,c)
	return Duel.GetMatchingGroupCount(Card.IsFaceup,0,LOCATION_EXTRA,LOCATION_EXTRA,nil)*100
end
function c98076754.indval(e,re,rp)
	return rp~=e:GetHandlerPlayer()
end
function c98076754.tdcon(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetMatchingGroup(c98076754.cfilter,tp,LOCATION_EXTRA,0,nil):GetClassCount(Card.GetCode)==10
end
function c98076754.tdcost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():IsAbleToGraveAsCost() end
	Duel.SendtoGrave(e:GetHandler(),REASON_COST)
end
function c98076754.filter(c)
	return c:IsAbleToDeck() and not c:IsHasEffect(EFFECT_NECRO_VALLEY)
end
function c98076754.tdtg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c98076754.filter,tp,0,LOCATION_HAND+LOCATION_ONFIELD+LOCATION_GRAVE,1,nil) end
	local g=Duel.GetMatchingGroup(c98076754.filter,tp,0,LOCATION_HAND+LOCATION_ONFIELD+LOCATION_GRAVE,nil)
	Duel.SetOperationInfo(0,CATEGORY_TODECK,g,g:GetCount(),0,0)
end
function c98076754.tdop(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetMatchingGroup(c98076754.filter,tp,0,LOCATION_HAND+LOCATION_ONFIELD+LOCATION_GRAVE,nil)
	Duel.SendtoDeck(g,nil,2,REASON_EFFECT)
end
