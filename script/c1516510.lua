--ルーンアイズ・ペンデュラム・ドラゴン
function c1516510.initial_effect(c)
	c:EnableReviveLimit()
	--fusion material
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE)
	e1:SetCode(EFFECT_FUSION_MATERIAL)
	e1:SetCondition(aux.FConditionCodeFun(16178681,aux.FilterBoolFunction(Card.IsRace,RACE_SPELLCASTER),1,true,false))
	e1:SetOperation(c1516510.fsop)
	c:RegisterEffect(e1)
end
c1516510.material_count=1
c1516510.material={16178681}
function c1516510.imfilter(c)
	return c:IsLocation(LOCATION_MZONE) and c:GetSummonType()==SUMMON_TYPE_PENDULUM
end
function c1516510.fsop(e,tp,eg,ep,ev,re,r,rp,gc,chkf)
	local g1=nil
	local g2=nil
	if gc then
		g1=Group.FromCards(gc)
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
		g2=eg:FilterSelect(tp,Card.IsRace,1,1,nil,RACE_SPELLCASTER)
		Duel.SetFusionMaterial(g2)
		g1:Merge(g2)
	else
		local sg1=Group.CreateGroup()
		local sg2=Group.CreateGroup()
		local fs=false
		local tc=eg:GetFirst()
		while tc do
			if tc:IsCode(16178681) or tc:IsHasEffect(EFFECT_FUSION_SUBSTITUTE) then sg1:AddCard(tc) end
			if tc:IsRace(RACE_SPELLCASTER) then
				sg2:AddCard(tc)
				if aux.FConditionCheckF(tc,chkf) then fs=true end
			end
			tc=eg:GetNext()
		end
		if sg2:GetCount()==1 then
			sg1:Sub(sg2)
		end
		if chkf~=PLAYER_NONE then
			Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
			if fs then g1=sg1:Select(tp,1,1,nil)
			else g1=sg1:FilterSelect(tp,aux.FConditionCheckF,1,1,nil,chkf) end
			local tc1=g1:GetFirst()
			sg2:RemoveCard(tc1)
			if aux.FConditionCheckF(tc1,chkf) or sg2:GetCount()==1 then
				Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
				g2=sg2:Select(tp,1,1,tc1)
			else
				Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
				g2=sg2:FilterSelect(tp,aux.FConditionCheckF,1,1,tc1,chkf)
			end
		else
			Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
			g1=sg1:Select(tp,1,1,nil)
			Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
			g2=sg2:Select(tp,1,1,g1:GetFirst())
		end
		g1:Merge(g2)
		Duel.SetFusionMaterial(g1)
	end
	local c=e:GetHandler()
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_EXTRA_ATTACK)
	e1:SetProperty(EFFECT_FLAG_CLIENT_HINT)
	if g2:GetFirst():GetOriginalLevel()<5 then
		e1:SetDescription(aux.Stringid(1516510,0))
		e1:SetValue(1)
	else
		e1:SetDescription(aux.Stringid(1516510,1))
		e1:SetValue(2)
	end
	e1:SetReset(RESET_EVENT+0xfe0000)
	c:RegisterEffect(e1)
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_SINGLE)
	e2:SetCode(EFFECT_CANNOT_DIRECT_ATTACK)
	e2:SetCondition(c1516510.dircon)
	e2:SetReset(RESET_EVENT+0xfe0000)
	c:RegisterEffect(e2)
	local e3=e2:Clone()
	e3:SetCode(EFFECT_CANNOT_ATTACK)
	e3:SetCondition(c1516510.atkcon)
	c:RegisterEffect(e3)
	if g1:IsExists(c1516510.imfilter,1,nil) then
		local e4=Effect.CreateEffect(c)
		e4:SetType(EFFECT_TYPE_SINGLE)
		e4:SetCode(EFFECT_IMMUNE_EFFECT)
		e4:SetValue(c1516510.efilter)
		e4:SetReset(RESET_EVENT+0xfe0000+RESET_PHASE+RESET_END)
		c:RegisterEffect(e4)
	end
end
function c1516510.dircon(e)
	return e:GetHandler():GetAttackAnnouncedCount()>0
end
function c1516510.atkcon(e)
	return e:GetHandler():IsDirectAttacked()
end
function c1516510.efilter(e,re)
	return e:GetHandlerPlayer()~=re:GetHandlerPlayer()
end
