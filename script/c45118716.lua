--マジック・リサイクラー
function c45118716.initial_effect(c)
	--todeck
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(45118716,0))
	e1:SetCategory(CATEGORY_TODECK)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_O)
	e1:SetCode(EVENT_ATTACK_ANNOUNCE)
	e1:SetRange(LOCATION_GRAVE)
	e1:SetCondition(c45118716.condition)
	e1:SetCost(c45118716.cost)
	e1:SetTarget(c45118716.target)
	e1:SetOperation(c45118716.operation)
	c:RegisterEffect(e1)
end
function c45118716.condition(e,tp,eg,ep,ev,re,r,rp)
	return ep~=tp
end
function c45118716.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():IsAbleToRemoveAsCost() end
	Duel.Remove(e:GetHandler(),POS_FACEUP,REASON_COST)
end
function c45118716.filter(c)
	return c:IsType(TYPE_SPELL) and c:IsAbleToDeck()
end
function c45118716.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsControler(tp) and chkc:IsLocation(LOCATION_GRAVE) and c45118716.filter(chkc) end
	if chk==0 then return Duel.IsPlayerCanDiscardDeck(tp,1)
		and Duel.IsExistingTarget(c45118716.filter,tp,LOCATION_GRAVE,0,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TODECK)
	local g=Duel.SelectTarget(tp,c45118716.filter,tp,LOCATION_GRAVE,0,1,1,nil)
	Duel.SetOperationInfo(0,CATEGORY_TODECK,g,1,0,0)
end
function c45118716.operation(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if Duel.DiscardDeck(tp,1,REASON_EFFECT)>0 and tc:IsRelateToEffect(e) then
		Duel.SendtoDeck(tc,nil,1,REASON_EFFECT)
	end
end
