--急転直下
function c79718768.initial_effect(c)
	--special summon
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	c:RegisterEffect(e1)
	--destroy
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_CONTINUOUS+EFFECT_TYPE_FIELD)
	e2:SetCode(EVENT_BECOME_TARGET)
	e2:SetRange(LOCATION_SZONE)
	e2:SetOperation(c79718768.desop1)
	c:RegisterEffect(e2)
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_CONTINUOUS+EFFECT_TYPE_FIELD)
	e3:SetCode(EVENT_CHAIN_SOLVED)
	e3:SetRange(LOCATION_SZONE)
	e3:SetOperation(c79718768.desop2)
	c:RegisterEffect(e3)
	e3:SetLabelObject(e2)
	--to deck
	local e4=Effect.CreateEffect(c)
	e4:SetDescription(aux.Stringid(79718768,0))
	e4:SetCategory(CATEGORY_TOHAND)
	e4:SetType(EFFECT_TYPE_TRIGGER_F+EFFECT_TYPE_SINGLE)
	e4:SetCode(EVENT_TO_GRAVE)
	e4:SetCondition(c79718768.retcon)
	e4:SetOperation(c79718768.retop)
	c:RegisterEffect(e4)
end
function c79718768.desop1(e,tp,eg,ep,ev,re,r,rp)
	if rp==tp or not re:IsHasProperty(EFFECT_FLAG_CARD_TARGET)
		or not eg:IsExists(Card.IsLocation,1,nil,LOCATION_GRAVE) then
		e:SetLabelObject(nil)
	else e:SetLabelObject(re) end
end
function c79718768.desop2(e,tp,eg,ep,ev,re,r,rp)
	local pe=e:GetLabelObject():GetLabelObject()
	if pe and pe==re then
		Duel.Destroy(e:GetHandler(),REASON_EFFECT)
	end
end
function c79718768.retcon(e,tp,eg,ep,ev,re,r,rp)
	return re:GetHandler()==e:GetHandler()
end
function c79718768.retop(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetFieldGroup(tp,0,LOCATION_GRAVE)
	Duel.SendtoDeck(g,nil,2,REASON_EFFECT)
end
