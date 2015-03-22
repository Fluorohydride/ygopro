--エンシェント・クリムゾン・エイプ
function c50781944.initial_effect(c)
	--lp rec
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(50781944,0))
	e1:SetCategory(CATEGORY_RECOVER)
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_F)
	e1:SetCode(EVENT_TO_GRAVE)
	e1:SetRange(LOCATION_MZONE)
	e1:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e1:SetCondition(c50781944.reccon)
	e1:SetTarget(c50781944.rectg)
	e1:SetOperation(c50781944.recop)
	c:RegisterEffect(e1)
end
function c50781944.filter(c,tp)
	return c:IsReason(REASON_DESTROY) and c:IsPreviousLocation(LOCATION_MZONE) and 
		c:GetPreviousControler()==tp and c:IsType(TYPE_MONSTER)
end
function c50781944.reccon(e,tp,eg,ep,ev,re,r,rp)
	return eg:IsExists(c50781944.filter,1,nil,tp)
end
function c50781944.rectg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetTargetPlayer(tp)
	Duel.SetTargetParam(1000)
	Duel.SetOperationInfo(0,CATEGORY_RECOVER,nil,0,tp,1000)
end
function c50781944.recop(e,tp,eg,ep,ev,re,r,rp)
	local p,d=Duel.GetChainInfo(0,CHAININFO_TARGET_PLAYER,CHAININFO_TARGET_PARAM)
	Duel.Recover(p,d,REASON_EFFECT)
end
