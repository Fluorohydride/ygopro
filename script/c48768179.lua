--黒蠍－強力のゴーグ
function c48768179.initial_effect(c)
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(48768179,0))
	e1:SetCategory(CATEGORY_TODECK+CATEGORY_TOGRAVE)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_O)
	e1:SetCode(EVENT_BATTLE_DAMAGE)
	e1:SetCondition(c48768179.condition)
	e1:SetTarget(c48768179.target)
	e1:SetOperation(c48768179.operation)
	c:RegisterEffect(e1)
end
function c48768179.condition(e,tp,eg,ep,ev,re,r,rp)
	return ep~=tp
end
function c48768179.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsOnField() and chkc:IsAbleToDeck() end
	if chk==0 then return Duel.IsPlayerCanDiscardDeck(1-tp,1)
		or Duel.IsExistingTarget(Card.IsAbleToDeck,tp,0,LOCATION_MZONE,1,nil) end
	local op=0
	Duel.Hint(HINT_SELECTMSG,tp,aux.Stringid(48768179,0))
	if Duel.IsExistingTarget(Card.IsAbleToDeck,tp,0,LOCATION_MZONE,1,nil)
		and Duel.IsPlayerCanDiscardDeck(1-tp,1) then
		op=Duel.SelectOption(tp,aux.Stringid(48768179,1),aux.Stringid(48768179,2))
	elseif Duel.IsPlayerCanDiscardDeck(1-tp,1) then
		Duel.SelectOption(tp,aux.Stringid(48768179,2))
		op=1
	else
		Duel.SelectOption(tp,aux.Stringid(48768179,1))
		op=0
	end
	e:SetLabel(op)
	if op==0 then
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TODECK)
		local g=Duel.SelectTarget(tp,Card.IsAbleToDeck,tp,0,LOCATION_MZONE,1,1,nil)
		Duel.SetOperationInfo(0,CATEGORY_TODECK,g,1,0,0)
		e:SetProperty(EFFECT_FLAG_CARD_TARGET)
	else
		Duel.SetOperationInfo(0,CATEGORY_DECKDES,0,0,1-tp,1)
		e:SetProperty(0)
	end
end
function c48768179.operation(e,tp,eg,ep,ev,re,r,rp)
	if e:GetLabel()==0 then
		local tc=Duel.GetFirstTarget()
		if tc and tc:IsRelateToEffect(e) then
			Duel.SendtoDeck(tc,nil,0,REASON_EFFECT)
		end
	else
		Duel.DiscardDeck(1-tp,1,REASON_EFFECT)
	end
end
