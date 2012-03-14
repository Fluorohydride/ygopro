--覚醒戦士 クーフーリン
function c10789972.initial_effect(c)
	c:EnableReviveLimit()
	--atkup
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(10789972,0))
	e1:SetCategory(CATEGORY_ATKCHANGE)
	e1:SetType(EFFECT_TYPE_IGNITION)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCountLimit(1)
	e1:SetCost(c10789972.cost)
	e1:SetOperation(c10789972.operation)
	c:RegisterEffect(e1)
end
function c10789972.cfilter(c)
	return c:IsType(TYPE_NORMAL) and c:IsAbleToRemoveAsCost()
end
function c10789972.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c10789972.cfilter,tp,LOCATION_GRAVE,0,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_REMOVE)
	local g=Duel.SelectMatchingCard(tp,c10789972.cfilter,tp,LOCATION_GRAVE,0,1,1,nil)
	e:SetLabel(g:GetFirst():GetAttack())
	Duel.Remove(g,POS_FACEUP,REASON_COST)
end
function c10789972.operation(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if e:GetLabel()~=0 and c:IsFaceup() and c:IsRelateToEffect(e) then
		local e1=Effect.CreateEffect(c)
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_UPDATE_ATTACK)
		e1:SetProperty(EFFECT_FLAG_COPY_INHERIT)
		e1:SetValue(e:GetLabel())
		e1:SetReset(RESET_EVENT+0x1ff0000+RESET_PHASE+PHASE_STANDBY,2)
		c:RegisterEffect(e1)
	end
end
