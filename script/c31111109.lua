--E・HERO ゴッド・ネオス
function c31111109.initial_effect(c)
	--fusion material
	c:EnableReviveLimit()
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE)
	e1:SetCode(EFFECT_FUSION_MATERIAL)
	e1:SetCondition(c31111109.fuscon)
	e1:SetOperation(c31111109.fusop)
	c:RegisterEffect(e1)
	--copy
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(31111109,0))
	e2:SetCategory(CATEGORY_ATKCHANGE)
	e2:SetType(EFFECT_TYPE_IGNITION)
	e2:SetRange(LOCATION_MZONE)
	e2:SetCountLimit(1)
	e2:SetTarget(c31111109.copytg)
	e2:SetOperation(c31111109.copyop)
	c:RegisterEffect(e2)
	--spsummon condition
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_SINGLE)
	e3:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE)
	e3:SetCode(EFFECT_SPSUMMON_CONDITION)
	e3:SetValue(c31111109.splimit)
	c:RegisterEffect(e3)
end
function c31111109.fuscon(e,g,gc,chkf)
	if g==nil then return false end
	if gc then return false end
	local g1=g:Filter(Card.IsSetCard,nil,0x9)
	local c1=g1:GetCount()
	local g2=g:Filter(Card.IsSetCard,nil,0x1f)
	local c2=g2:GetCount()
	local g3=g:Filter(Card.IsSetCard,nil,0x8)
	local c3=g3:GetCount()
	local ag=g1:Clone()
	ag:Merge(g2)
	ag:Merge(g3)
	if chkf~=PLAYER_NONE and not ag:IsExists(aux.FConditionCheckF,1,nil,chkf) then return false end
	return c1>0 and c2>0 and c3>0 and ag:GetCount()>=5 and (c1>1 or c3>1 or g1:GetFirst()~=g3:GetFirst())
end
function c31111109.fusop(e,tp,eg,ep,ev,re,r,rp,gc,chkf)
	if gc then return end
	local g1=eg:Filter(Card.IsSetCard,nil,0x9)
	local g2=eg:Filter(Card.IsSetCard,nil,0x1f)
	local g3=eg:Filter(Card.IsSetCard,nil,0x8)
	local ag=g1:Clone()
	ag:Merge(g2)
	ag:Merge(g3)
	local tc=nil local f1=0 local f2=0 local f3=0
	local mg=Group.CreateGroup()
	for i=1,5 do
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
		if i==1 and chkf~=PLAYER_NONE then
			tc=ag:FilterSelect(tp,aux.FConditionCheckF,1,1,nil,chkf):GetFirst()
		else tc=ag:Select(tp,1,1,nil):GetFirst() end
		if g1:IsContains(tc) then
			g1:RemoveCard(tc)
			if g3:IsContains(tc) then g3:RemoveCard(tc) f1=f1+1 f3=f3+1
			else f1=f1+2 end
		elseif g2:IsContains(tc) then g2:RemoveCard(tc) f2=f2+1
		else g3:RemoveCard(tc) f3=f3+2 end
		ag:RemoveCard(tc)
		mg:AddCard(tc)
		if i==3 then
			if f1==0 and f2==0 then ag:Sub(g3)
			elseif f1==0 and f3==0 then ag:Sub(g2)
			elseif f2==0 and f3==0 then ag:Sub(g1) end
		end
		if i==4 then
			if f1==0 then ag=g1
			elseif f2==0 then ag=g2
			elseif f3==0 then ag=g3
			elseif f1==1 and f3==1 then ag:Sub(g2) end
		end
	end
	Duel.SetFusionMaterial(mg)
end
function c31111109.splimit(e,se,sp,st)
	return bit.band(st,SUMMON_TYPE_FUSION)==SUMMON_TYPE_FUSION
end
function c31111109.filter(c)
	return (c:IsSetCard(0x9) or c:IsSetCard(0x1f) or c:IsSetCard(0x8)) and c:IsType(TYPE_MONSTER)
		and not c:IsHasEffect(EFFECT_FORBIDDEN) and c:IsAbleToRemove()
end
function c31111109.copytg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_GRAVE) and chkc:IsControler(tp) and c31111109.filter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c31111109.filter,tp,LOCATION_GRAVE,0,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_REMOVE)
	local g=Duel.SelectTarget(tp,c31111109.filter,tp,LOCATION_GRAVE,0,1,1,nil)
	Duel.SetOperationInfo(0,CATEGORY_REMOVE,g,1,0,0)
end
function c31111109.copyop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local tc=Duel.GetFirstTarget()
	if c:IsRelateToEffect(e) and c:IsFaceup() and tc:IsRelateToEffect(e) then
		if Duel.Remove(tc,POS_FACEUP,REASON_EFFECT)~=1 then	return end
		local code=tc:GetOriginalCode()
		local cid=c:CopyEffect(code,RESET_EVENT+0x1fe0000+RESET_PHASE+RESET_END,1)
		local e1=Effect.CreateEffect(c)
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetProperty(EFFECT_FLAG_COPY_INHERIT)
		e1:SetCode(EFFECT_UPDATE_ATTACK)
		e1:SetValue(500)
		e1:SetReset(RESET_EVENT+0x1ff0000)
		c:RegisterEffect(e1)
		local e2=Effect.CreateEffect(c)
		e2:SetDescription(aux.Stringid(31111109,1))
		e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		e2:SetCode(EVENT_PHASE+PHASE_END)
		e2:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE)
		e2:SetCountLimit(1)
		e2:SetRange(LOCATION_MZONE)
		e2:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+RESET_END)
		e2:SetLabel(cid)
		e2:SetOperation(c31111109.rstop)
		c:RegisterEffect(e2)
	end
end
function c31111109.rstop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local cid=e:GetLabel()
	c:ResetEffect(cid,RESET_COPY)
	Duel.HintSelection(Group.FromCards(c))
	Duel.Hint(HINT_OPSELECTED,1-tp,e:GetDescription())
end
