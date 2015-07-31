--サクリボー
function c50185950.initial_effect(c)
	--draw
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(50185950,0))
	e1:SetCategory(CATEGORY_DRAW)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e1:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e1:SetCode(EVENT_RELEASE)
	e1:SetTarget(c50185950.drtg)
	e1:SetOperation(c50185950.drop)
	c:RegisterEffect(e1)
	--destroy replace
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_CONTINUOUS+EFFECT_TYPE_FIELD)
	e2:SetCode(EFFECT_DESTROY_REPLACE)
	e2:SetRange(LOCATION_GRAVE)
	e2:SetTarget(c50185950.reptg)
	e2:SetValue(c50185950.repval)
	e2:SetOperation(c50185950.repop)
	c:RegisterEffect(e2)
end
function c50185950.drtg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetTargetPlayer(tp)
	Duel.SetTargetParam(1)
	Duel.SetOperationInfo(0,CATEGORY_DRAW,nil,0,tp,1)
end
function c50185950.drop(e,tp,eg,ep,ev,re,r,rp)
	local p,d=Duel.GetChainInfo(0,CHAININFO_TARGET_PLAYER,CHAININFO_TARGET_PARAM)
	Duel.Draw(p,d,REASON_EFFECT)
end
function c50185950.filter(c,tp)
	return c:IsControler(tp) and c:IsLocation(LOCATION_MZONE)
		and c:IsReason(REASON_BATTLE) and not c:IsReason(REASON_REPLACE)
end
function c50185950.reptg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return eg:IsExists(c50185950.filter,1,nil,tp) and e:GetHandler():IsAbleToRemove() end
	return Duel.SelectYesNo(tp,aux.Stringid(50185950,1))
end
function c50185950.repval(e,c)
	return c50185950.filter(c,e:GetHandlerPlayer())
end
function c50185950.repop(e,tp,eg,ep,ev,re,r,rp)
	Duel.Remove(e:GetHandler(),POS_FACEUP,REASON_EFFECT+REASON_REPLACE)
end
