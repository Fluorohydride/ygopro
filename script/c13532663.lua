--ダミー・ゴーレム
function c13532663.initial_effect(c)
	--flip
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(13532663,0))
	e1:SetCategory(CATEGORY_CONTROL)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_FLIP)
	e1:SetTarget(c13532663.target)
	e1:SetOperation(c13532663.operation)
	c:RegisterEffect(e1)
end
function c13532663.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetOperationInfo(0,CATEGORY_CONTROL,nil,0,0,0)
end
function c13532663.operation(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if c:IsStatus(STATUS_BATTLE_DESTROYED) or not c:IsRelateToEffect(e) or not c:IsAbleToChangeControler()
		or not Duel.IsExistingMatchingCard(Card.IsAbleToChangeControler,tp,0,LOCATION_MZONE,1,nil) then
		return
	end
	Duel.Hint(HINT_SELECTMSG,1-tp,HINTMSG_CONTROL)
	local g=Duel.SelectMatchingCard(1-tp,Card.IsAbleToChangeControler,1-tp,LOCATION_MZONE,0,1,1,nil)
	Duel.SwapControl(c,g:GetFirst(),0,0)
end
