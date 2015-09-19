--PSYフレーム・オーバーロード
function c36970611.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(0,TIMING_END_PHASE)
	e1:SetTarget(c36970611.target1)
	c:RegisterEffect(e1)
	--remove
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(36970611,0))
	e2:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e2:SetType(EFFECT_TYPE_QUICK_O)
	e2:SetRange(LOCATION_SZONE)
	e2:SetCode(EVENT_FREE_CHAIN)
	e2:SetHintTiming(0,TIMING_END_PHASE)
	e2:SetCost(c36970611.cost2)
	e2:SetTarget(c36970611.target2)
	e2:SetOperation(c36970611.operation)
	c:RegisterEffect(e2)
	--to hand
	local e3=Effect.CreateEffect(c)
	e3:SetDescription(aux.Stringid(36970611,1))
	e3:SetCategory(CATEGORY_TOHAND+CATEGORY_SEARCH)
	e3:SetType(EFFECT_TYPE_QUICK_O)
	e3:SetRange(LOCATION_GRAVE)
	e3:SetCode(EVENT_FREE_CHAIN)
	e3:SetCondition(aux.exccon)
	e3:SetCost(c36970611.thcost)
	e3:SetTarget(c36970611.thtg)
	e3:SetOperation(c36970611.thop)
	c:RegisterEffect(e3)
end
function c36970611.cfilter(c)
	return c:IsSetCard(0xc1) and c:IsType(TYPE_MONSTER) and (c:IsLocation(LOCATION_HAND) or c:IsFaceup()) and c:IsAbleToRemoveAsCost()
		and Duel.IsExistingTarget(Card.IsAbleToRemove,0,LOCATION_ONFIELD,LOCATION_ONFIELD,1,c)
end
function c36970611.target1(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return c36970611.target2(e,tp,eg,ep,ev,re,r,rp,chk,chkc) end
	if chk==0 then return true end
	if c36970611.cost2(e,tp,eg,ep,ev,re,r,rp,0) and c36970611.target2(e,tp,eg,ep,ev,re,r,rp,0,chkc)
		and Duel.SelectYesNo(tp,aux.Stringid(36970611,2)) then
		e:SetProperty(EFFECT_FLAG_CARD_TARGET)
		e:SetOperation(c36970611.operation)
		c36970611.cost2(e,tp,eg,ep,ev,re,r,rp,1)
		c36970611.target2(e,tp,eg,ep,ev,re,r,rp,1,chkc)
	else
		e:SetProperty(0)
		e:SetOperation(nil)
	end
end
function c36970611.cost2(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c36970611.cfilter,tp,LOCATION_HAND+LOCATION_MZONE,0,1,nil) and e:GetHandler():GetFlagEffect(36970611)==0 end
	Duel.Hint(HINT_SELECTMSG,tp,aux.Stringid(36970611,3))
	local cg=Duel.SelectMatchingCard(tp,c36970611.cfilter,tp,LOCATION_HAND+LOCATION_MZONE,0,1,1,nil)
	Duel.Remove(cg,POS_FACEUP,REASON_COST)
	e:GetHandler():RegisterFlagEffect(36970611,RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END,0,1)
end
function c36970611.target2(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsOnField() and chkc:IsAbleToRemove() end
	if chk==0 then return true end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_REMOVE)
	local g=Duel.SelectTarget(tp,Card.IsAbleToRemove,tp,LOCATION_ONFIELD,LOCATION_ONFIELD,1,1,nil)
	Duel.SetOperationInfo(0,CATEGORY_REMOVE,g,1,0,0)
end
function c36970611.operation(e,tp,eg,ep,ev,re,r,rp)
	if not e:GetHandler():IsRelateToEffect(e) then return end
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) then
		Duel.Remove(tc,POS_FACEDOWN,REASON_EFFECT)
	end
end
function c36970611.thcost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():IsAbleToRemoveAsCost() end
	Duel.Remove(e:GetHandler(),POS_FACEUP,REASON_COST)
end
function c36970611.thfilter(c)
	return c:IsSetCard(0xc1) and not c:IsCode(36970611) and c:IsAbleToHand()
end
function c36970611.thtg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c36970611.thfilter,tp,LOCATION_DECK,0,1,nil) end
	Duel.SetOperationInfo(0,CATEGORY_TOHAND,nil,1,tp,LOCATION_DECK)
end
function c36970611.thop(e,tp,eg,ep,ev,re,r,rp)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_ATOHAND)
	local g=Duel.SelectMatchingCard(tp,c36970611.thfilter,tp,LOCATION_DECK,0,1,1,nil)
	if g:GetCount()>0 then
		Duel.SendtoHand(g,nil,REASON_EFFECT)
		Duel.ConfirmCards(1-tp,g)
	end
end
