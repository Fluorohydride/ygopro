--イモータル·ルーラー
function c32485518.initial_effect(c)
	--salvage
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(32485518,0))
	e1:SetCategory(CATEGORY_TOHAND)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetType(EFFECT_TYPE_IGNITION)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCost(c32485518.cost)
	e1:SetTarget(c32485518.target)
	e1:SetOperation(c32485518.operation)
	c:RegisterEffect(e1)
end
function c32485518.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():IsReleaseable() end
	Duel.Release(e:GetHandler(),REASON_COST)
end
function c32485518.filter(c)
	return c:GetCode()==4064256 and c:IsAbleToHand()
end
function c32485518.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_GRAVE) and chkc:IsControler(tp) and c32485518.filter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c32485518.filter,tp,LOCATION_GRAVE,0,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_ATOHAND)
	local g=Duel.SelectTarget(tp,c32485518.filter,tp,LOCATION_GRAVE,0,1,1,nil)
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,g,g:GetCount(),0,0)
end
function c32485518.operation(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc and tc:IsRelateToEffect(e) then
		Duel.SendtoHand(tc,nil,REASON_EFECT)
		Duel.ConfirmCards(1-tp,tc)
	end
end
