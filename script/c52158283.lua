--先史遺産コロッサル・ヘッド
function c52158283.initial_effect(c)
	--adchange
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(52158283,0))
	e1:SetCategory(CATEGORY_POSITION)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetType(EFFECT_TYPE_IGNITION)
	e1:SetRange(LOCATION_GRAVE)
	e1:SetCountLimit(1,52158283)
	e1:SetCost(c52158283.cost)
	e1:SetTarget(c52158283.target)
	e1:SetOperation(c52158283.operation)
	c:RegisterEffect(e1)
end
function c52158283.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():IsAbleToRemoveAsCost() end
	Duel.Remove(e:GetHandler(),POS_FACEUP,REASON_COST)
end
function c52158283.filter(c)
	return c:IsAttackPos() and c:IsLevelAbove(3)
end
function c52158283.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and c52158283.filter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c52158283.filter,tp,LOCATION_MZONE,LOCATION_MZONE,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_POSCHANGE)
	local g=Duel.SelectTarget(tp,c52158283.filter,tp,LOCATION_MZONE,LOCATION_MZONE,1,1,nil)
	Duel.SetOperationInfo(0,CATEGORY_POSITION,g,1,0,0)
end
function c52158283.operation(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) then
		if tc:IsAttackPos() then
			local pos=0
			if tc:IsCanTurnSet() then
				pos=Duel.SelectPosition(tp,tc,POS_DEFENCE)
			else
				pos=Duel.SelectPosition(tp,tc,POS_FACEUP_DEFENCE)
			end
			Duel.ChangePosition(tc,pos)
		else
			Duel.ChangePosition(tc,0,0,POS_FACEDOWN_DEFENCE,POS_FACEUP_DEFENCE)
		end
	end
end
