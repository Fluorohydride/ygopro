--ダメージ・イーター
function c60741115.initial_effect(c)
	--
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(60741115,0))
	e1:SetType(EFFECT_TYPE_QUICK_O)
	e1:SetCode(EVENT_CHAINING)
	e1:SetRange(LOCATION_GRAVE)
	e1:SetCondition(c60741115.condition)
	e1:SetCost(c60741115.cost)
	e1:SetOperation(c60741115.operation)
	c:RegisterEffect(e1)
end
function c60741115.condition(e,tp,eg,ep,ev,re,r,rp)
	if rp==tp or Duel.GetTurnPlayer()==tp then return false end
	local ex,cg,ct,cp,cv=Duel.GetOperationInfo(ev,CATEGORY_DAMAGE)
	if ex then return true end
	ex,cg,ct,cp,cv=Duel.GetOperationInfo(ev,CATEGORY_RECOVER)
	if not ex then return false end
	if cp~=PLAYER_ALL then return Duel.IsPlayerAffectedByEffect(cp,EFFECT_REVERSE_RECOVER)
	else return Duel.IsPlayerAffectedByEffect(0,EFFECT_REVERSE_RECOVER)
		or Duel.IsPlayerAffectedByEffect(1,EFFECT_REVERSE_RECOVER)
	end
end
function c60741115.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():IsAbleToRemoveAsCost() end
	Duel.Remove(e:GetHandler(),POS_FACEUP,REASON_COST)
end
function c60741115.operation(e,tp,eg,ep,ev,re,r,rp)
	local cid=Duel.GetChainInfo(ev,CHAININFO_CHAIN_ID)
	--damage conversion
	local e1=Effect.CreateEffect(e:GetHandler())
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e1:SetCode(EFFECT_REVERSE_DAMAGE)
	e1:SetTargetRange(1,1)
	e1:SetValue(c60741115.rev)
	e1:SetReset(RESET_CHAIN)
	e1:SetLabel(cid)
	Duel.RegisterEffect(e1,tp)
end
function c60741115.rev(e,re,r,rp,rc)
	local cc=Duel.GetCurrentChain()
	if cc==0 or bit.band(r,REASON_EFFECT)==0 then return false end
	local cid=Duel.GetChainInfo(0,CHAININFO_CHAIN_ID)
	return cid==e:GetLabel()
end
