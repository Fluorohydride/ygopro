--黒蠍－罠はずしのクリフ
function c6967870.initial_effect(c)
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(6967870,0))
	e1:SetCategory(CATEGORY_DESTROY+CATEGORY_DECKDES)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_O)
	e1:SetCode(EVENT_BATTLE_DAMAGE)
	e1:SetCondition(c6967870.condition)
	e1:SetTarget(c6967870.target)
	e1:SetOperation(c6967870.operation)
	c:RegisterEffect(e1)
end
function c6967870.condition(e,tp,eg,ep,ev,re,r,rp)
	return ep~=tp
end
function c6967870.filter(c)
	return c:IsType(TYPE_TRAP+TYPE_SPELL) and c:IsDestructable()
end
function c6967870.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsOnField() and c6967870.filter(chkc) end
	if chk==0 then return Duel.IsPlayerCanDiscardDeck(1-tp,2)
		or Duel.IsExistingTarget(c6967870.filter,tp,LOCATION_ONFIELD,LOCATION_ONFIELD,1,nil) end
	local op=0
	if Duel.IsExistingTarget(c6967870.filter,tp,LOCATION_ONFIELD,LOCATION_ONFIELD,1,nil)
		and Duel.IsPlayerCanDiscardDeck(1-tp,2) then
		op=Duel.SelectOption(tp,aux.Stringid(6967870,1),aux.Stringid(6967870,2))
	elseif Duel.IsPlayerCanDiscardDeck(1-tp,2) then
		Duel.SelectOption(tp,aux.Stringid(6967870,2))
		op=1
	else
		Duel.SelectOption(tp,aux.Stringid(6967870,1))
		op=0
	end
	e:SetLabel(op)
	if op==0 then
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_DESTROY)
		local g=Duel.SelectTarget(tp,c6967870.filter,tp,LOCATION_ONFIELD,LOCATION_ONFIELD,1,1,nil)
		Duel.SetOperationInfo(0,CATEGORY_DESTROY,g,1,0,0)
		e:SetProperty(EFFECT_FLAG_CARD_TARGET)
	else
		Duel.SetOperationInfo(0,CATEGORY_DECKDES,0,0,1-tp,1)
		e:SetProperty(0)
	end
end
function c6967870.operation(e,tp,eg,ep,ev,re,r,rp)
	if e:GetLabel()==0 then
		local tc=Duel.GetFirstTarget()
		if tc and tc:IsRelateToEffect(e) then
			Duel.Destroy(tc,REASON_EFFECT)
		end
	else
		Duel.DiscardDeck(1-tp,2,REASON_EFFECT)
	end
end
