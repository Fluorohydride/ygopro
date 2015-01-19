--ダーク・リペアラー
function c76614003.initial_effect(c)
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(76614003,0))
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e1:SetCode(EVENT_TO_GRAVE)
	e1:SetCondition(c76614003.condition)
	e1:SetOperation(c76614003.operation)
	c:RegisterEffect(e1)
end
function c76614003.condition(e,tp,eg,ep,ev,re,r,rp)
	return e:GetHandler():IsPreviousLocation(LOCATION_ONFIELD) and e:GetHandler():GetPreviousControler()==tp
end
function c76614003.operation(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetFieldGroupCount(tp,LOCATION_DECK,0)==0 then return end
	local g=Duel.GetDecktopGroup(tp,1)
	Duel.ConfirmCards(tp,g)
	local tc=g:GetFirst()
	local opt=Duel.SelectOption(tp,aux.Stringid(76614003,1),aux.Stringid(76614003,2))
	if opt==1 then
		Duel.MoveSequence(tc,opt)
	end
end
