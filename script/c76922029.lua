--首領・ザルーグ
function c76922029.initial_effect(c)
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(76922029,0))
	e1:SetCategory(CATEGORY_HANDES+CATEGORY_DECKDES)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_O)
	e1:SetCode(EVENT_BATTLE_DAMAGE)
	e1:SetCondition(c76922029.condition)
	e1:SetTarget(c76922029.target)
	e1:SetOperation(c76922029.operation)
	c:RegisterEffect(e1)
end
function c76922029.condition(e,tp,eg,ep,ev,re,r,rp)
	return ep~=tp
end
function c76922029.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetFieldGroupCount(tp,0,LOCATION_HAND)>0 or Duel.IsPlayerCanDiscardDeck(1-tp,2) end
	local op=0
	Duel.Hint(HINT_SELECTMSG,tp,aux.Stringid(76922029,0))
	if Duel.GetFieldGroupCount(tp,0,LOCATION_HAND)>0 and Duel.IsPlayerCanDiscardDeck(1-tp,2) then
		op=Duel.SelectOption(tp,aux.Stringid(76922029,1),aux.Stringid(76922029,2))
	elseif Duel.GetFieldGroupCount(tp,0,LOCATION_HAND)>0 then
		Duel.SelectOption(tp,aux.Stringid(76922029,1))
		op=0
	else
		Duel.SelectOption(tp,aux.Stringid(76922029,2))
		op=1
	end
	e:SetLabel(op)
	if op==0 then Duel.SetOperationInfo(0,CATEGORY_HANDES,0,0,1-tp,1)
	else Duel.SetOperationInfo(0,CATEGORY_DECKDES,0,0,1-tp,2) end
end
function c76922029.operation(e,tp,eg,ep,ev,re,r,rp)
	if e:GetLabel()==0 then
		local g=Duel.GetFieldGroup(ep,LOCATION_HAND,0,nil)
		local sg=g:RandomSelect(ep,1)
		Duel.SendtoGrave(sg,REASON_DISCARD+REASON_EFFECT)
	else
		Duel.DiscardDeck(1-tp,2,REASON_EFFECT)
	end
end
