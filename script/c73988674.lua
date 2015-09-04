--三位一択
function c73988674.initial_effect(c)
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetTarget(c73988674.target)
	e1:SetOperation(c73988674.operation)
	c:RegisterEffect(e1)
end
function c73988674.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetFieldGroupCount(tp,LOCATION_EXTRA,LOCATION_EXTRA)>0 end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_CARDTYPE)
	local op=Duel.SelectOption(tp,aux.Stringid(73988674,0),aux.Stringid(73988674,1),aux.Stringid(73988674,2))
	e:SetLabel(op)
end
function c73988674.operation(e,tp,eg,ep,ev,re,r,rp)
	local g1=Duel.GetFieldGroup(tp,LOCATION_EXTRA,0)
	Duel.ConfirmCards(1-tp,g1)
	local g2=Duel.GetFieldGroup(tp,0,LOCATION_EXTRA)
	Duel.ConfirmCards(tp,g2)
	local tpe=0
	if e:GetLabel()==0 then tpe=TYPE_FUSION
	elseif e:GetLabel()==1 then tpe=TYPE_SYNCHRO
	else tpe=TYPE_XYZ end
	local ct1=g1:FilterCount(Card.IsType,nil,tpe)
	local ct2=g2:FilterCount(Card.IsType,nil,tpe)
	if ct1>ct2 then
		Duel.Recover(tp,3000,REASON_EFFECT)
	elseif ct1<ct2 then
		Duel.Recover(1-tp,3000,REASON_EFFECT)
	end
end
