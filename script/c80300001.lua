--Lightsworn Sanctuary
function c80300001.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	c:RegisterEffect(e1)
	--To Hand
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(80300001,1))
	e2:SetCategory(CATEGORY_TOHAND)
	e2:SetType(EFFECT_TYPE_IGNITION)
	e2:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e2:SetCountLimit(1)
	e2:SetRange(LOCATION_SZONE)
	e2:SetCost(c80300001.cost)
	e2:SetTarget(c80300001.target)
	e2:SetOperation(c80300001.activate)
	c:RegisterEffect(e2)
	--Add counter
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_CONTINUOUS+EFFECT_TYPE_FIELD)
	e3:SetCode(EVENT_TO_GRAVE)
	e3:SetRange(LOCATION_SZONE)
	e3:SetOperation(c80300001.acop)
	c:RegisterEffect(e3)
	--Destroy replace
	local e4=Effect.CreateEffect(c)
	e4:SetType(EFFECT_TYPE_CONTINUOUS+EFFECT_TYPE_FIELD)
	e4:SetCode(EFFECT_DESTROY_REPLACE)
	e4:SetRange(LOCATION_SZONE)
	e4:SetTarget(c80300001.desreptg)
	e4:SetValue(c80300001.value)
	e4:SetOperation(c80300001.desrepop)
	c:RegisterEffect(e4)
end
function c80300001.costfilter(c)
	return c:IsType(TYPE_MONSTER) and c:IsSetCard(0x38) and c:IsAbleToGraveAsCost() 
end
function c80300001.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c80300001.costfilter,tp,LOCATION_HAND,0,1,e:GetHandler()) end
	local g=Duel.GetFieldGroup(tp,LOCATION_HAND,0)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TOGRAVE)
	local sg=g:FilterSelect(tp,c80300001.costfilter,1,1,nil)
	e:SetLabelObject(sg:GetFirst())
	Duel.SendtoGrave(sg,REASON_COST)
end
function c80300001.tgfilter(c)
	return c:IsType(TYPE_MONSTER) and c:IsSetCard(0x38) and c:IsAbleToHand() 
end
function c80300001.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	local c=e:GetLabelObject()
	if chkc then return chkc:IsLocation(LOCATION_GRAVE) and chkc:IsControler(tp) and c80300001.tgfilter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c80300001.tgfilter,tp,LOCATION_GRAVE,0,1,c) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_ATOHAND)
	local sg=Duel.SelectTarget(tp,c80300001.tgfilter,tp,LOCATION_GRAVE,0,1,1,c)
	Duel.SetOperationInfo(0,CATEGORY_TOHAND,sg,sg:GetCount(),0,0)
end
function c80300001.activate(e,tp,eg,ep,ev,re,r,rp)
	if not e:GetHandler():IsRelateToEffect(e) then return end
	local tc=Duel.GetFirstTarget()
	if tc and tc:IsRelateToEffect(e) then
		Duel.SendtoHand(tc,nil,REASON_EFFECT)
		Duel.ConfirmCards(1-tp,tc)
	end
end
function c80300001.cfilter(c,tp)
	return c:GetPreviousLocation()==LOCATION_DECK and c:GetPreviousControler()==tp
end
function c80300001.acop(e,tp,eg,ep,ev,re,r,rp)
	if eg:IsExists(c80300001.cfilter,1,nil,tp) then
		e:GetHandler():AddCounter(0x5,1)
	end
end
function c80300001.dfilter(c,tp)
	return c:IsLocation(LOCATION_ONFIELD) and c:IsFaceup() and c:IsSetCard(0x38) and c:IsControler(tp)
end
function c80300001.desreptg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then
		local count=eg:FilterCount(c80300001.dfilter,nil,tp)*2
		e:SetLabel(count)
		return count>0 and Duel.IsCanRemoveCounter(tp,1,0,0x5,count,REASON_EFFECT)
	end
	return Duel.SelectYesNo(tp,aux.Stringid(80300001,1))
end
function c80300001.value(e,c)
	return c:IsOnField() and c:IsFaceup() and c:IsSetCard(0x38) and c:IsControler(e:GetHandlerPlayer())
end
function c80300001.desrepop(e,tp,eg,ep,ev,re,r,rp)
	local count=e:GetLabel()
	e:GetHandler():RemoveCounter(ep,0x5,count,REASON_EFFECT)
end
