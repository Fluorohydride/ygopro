--天空の泉
function c92223641.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	c:RegisterEffect(e1)
	--adjust
	local e2=Effect.CreateEffect(c)
	e2:SetCategory(CATEGORY_RECOVER)
	e2:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_O)
	e2:SetCode(EVENT_BATTLE_DESTROYED)
	e2:SetRange(LOCATION_SZONE)
	e2:SetCost(c92223641.cost)
	e2:SetTarget(c92223641.tg)
	e2:SetOperation(c92223641.op)
	c:RegisterEffect(e2)
end
function c92223641.filter(g,tp)
	local c=g:GetFirst()
	if c:IsControler(1-tp) then c=g:GetNext() end
	if c and c:IsAttribute(ATTRIBUTE_LIGHT) and c:IsLocation(LOCATION_GRAVE) then return c end
	return nil
end
function c92223641.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then
		local rc=c92223641.filter(eg,tp)
		return rc and rc:IsAbleToRemoveAsCost()
	end
	local rc=c92223641.filter(eg,tp)
	e:SetLabel(rc:GetAttack())
	Duel.Remove(rc,POS_FACEUP,REASON_EFFECT)
end
function c92223641.tg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetTargetPlayer(tp)
	Duel.SetTargetParam(e:GetLabel())
	Duel.SetOperationInfo(0,CATEGORY_RECOVER,nil,0,tp,e:GetLabel())
end
function c92223641.op(e,tp,eg,ep,ev,re,r,rp)
	if not e:GetHandler():IsRelateToEffect(e) then return end
	local p,d=Duel.GetChainInfo(0,CHAININFO_TARGET_PLAYER,CHAININFO_TARGET_PARAM)
	Duel.Recover(p,d,REASON_EFFECT)
end
