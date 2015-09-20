--Kyoutou Waterfront
function c56111151.initial_effect(c)
	c:SetCounterLimit(0x37,5)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	c:RegisterEffect(e1)
	--counter
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e2:SetCode(EVENT_TO_GRAVE)
	e2:SetRange(LOCATION_FZONE)
	e2:SetOperation(c56111151.counter)
	c:RegisterEffect(e2)
	--to hand
	local e3=Effect.CreateEffect(c)
	e3:SetCategory(CATEGORY_TOHAND+CATEGORY_SEARCH)
	e3:SetType(EFFECT_TYPE_IGNITION)
	e3:SetRange(LOCATION_FZONE)
	e3:SetCountLimit(1)
	e3:SetCondition(c56111151.thcon)
	e3:SetTarget(c56111151.thtg)
	e3:SetOperation(c56111151.thop)
	c:RegisterEffect(e3)
	--Destroy replace
	local e4=Effect.CreateEffect(c)
	e4:SetType(EFFECT_TYPE_CONTINUOUS+EFFECT_TYPE_SINGLE)
	e4:SetCode(EFFECT_DESTROY_REPLACE)
	e4:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e4:SetRange(LOCATION_FZONE)
	e4:SetTarget(c56111151.desreptg)
	e4:SetOperation(c56111151.desrepop)
	c:RegisterEffect(e4)
end
function c56111151.cfilter(c)
	return c:IsPreviousLocation(LOCATION_ONFIELD)
end
function c56111151.counter(e,tp,eg,ep,ev,re,r,rp)
	local ct=eg:FilterCount(c56111151.cfilter,nil)
	if ct>0 then
		e:GetHandler():AddCounter(0x37,ct)
	end
end
function c56111151.thfilter(c)
	return c:IsSetCard(0xd3) and c:IsType(TYPE_MONSTER) and c:IsAbleToHand()
end
function c56111151.thcon(e,tp,eg,ep,ev,re,r,rp)
	return e:GetHandler():GetCounter(0x37)>=3
end
function c56111151.thtg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c56111151.thfilter,tp,LOCATION_DECK,0,1,nil) end
	Duel.SetOperationInfo(0,CATEGORY_TOHAND,nil,1,tp,LOCATION_DECK)
end
function c56111151.thop(e,tp,eg,ep,ev,re,r,rp)
	if not e:GetHandler():IsRelateToEffect(e) then return end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_ATOHAND)
	local g=Duel.SelectMatchingCard(tp,c56111151.thfilter,tp,LOCATION_DECK,0,1,1,nil)
	if g:GetCount()>0 then
		Duel.SendtoHand(g,nil,REASON_EFFECT)
		Duel.ConfirmCards(1-tp,g)
	end
end
function c56111151.desreptg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return not e:GetHandler():IsReason(REASON_RULE)
		and e:GetHandler():IsCanRemoveCounter(tp,0x37,1,REASON_EFFECT) end
	return Duel.SelectYesNo(tp,aux.Stringid(56111151,0))
end
function c56111151.desrepop(e,tp,eg,ep,ev,re,r,rp)
	e:GetHandler():RemoveCounter(tp,0x37,1,REASON_EFFECT)
end
