--Ｍ・ＨＥＲＯ ダーク・ロウ
function c80100044.initial_effect(c)
	c:EnableReviveLimit()
	--spsummon condition
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE)
	e1:SetCode(EFFECT_SPSUMMON_CONDITION)
	e1:SetValue(aux.FALSE)
	c:RegisterEffect(e1)
	--remove
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD)
	e2:SetProperty(EFFECT_FLAG_SET_AVAILABLE+EFFECT_FLAG_IGNORE_RANGE)
	e2:SetCode(EFFECT_TO_GRAVE_REDIRECT)
	e2:SetRange(LOCATION_MZONE)
	e2:SetValue(LOCATION_REMOVED)
	e2:SetTarget(c80100044.rmtg)
	c:RegisterEffect(e2)
	--remove Hand
	local e3=Effect.CreateEffect(c)
	e3:SetCategory(CATEGORY_REMOVE)
	e3:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_O)
	e3:SetRange(LOCATION_MZONE)
	e3:SetCode(EVENT_TO_HAND)
	e3:SetCountLimit(1)
	e3:SetCondition(c80100044.condition)
	e3:SetTarget(c80100044.target)
	e3:SetOperation(c80100044.activate)
	c:RegisterEffect(e3)
end
function c80100044.rmtg(e,c)
	return c:GetOwner()~=e:GetOwnerPlayer() and not (c:IsType(TYPE_PENDULUM) and c:IsPreviousLocation(LOCATION_ONFIELD))
end
function c80100044.cfilter(c,tp)
	return c:IsControler(tp) and c:IsPreviousLocation(LOCATION_DECK) 
end
function c80100044.condition(e,tp,eg,ep,ev,re,r,rp)
	return eg:IsExists(c80100044.cfilter,1,nil,1-tp) and Duel.GetCurrentPhase()~=PHASE_DRAW
end
function c80100044.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetFieldGroupCount(tp,0,LOCATION_HAND)>0 end
	Duel.SetOperationInfo(0,CATEGORY_REMOVE,nil,1,0,0)
end
function c80100044.activate(e,tp,eg,ep,ev,re,r,rp)
	local sg=Duel.GetFieldGroup(tp,0,LOCATION_HAND)
	local dg=sg:RandomSelect(tp,1)
	Duel.Remove(dg,POS_FACEUP,REASON_EFFECT)
	Duel.ShuffleHand(1-tp)
end